#include "MantidQtWidgets/Common/Batch/JobTreeView.h"
#include "MantidQtWidgets/Common/Batch/AssertOrThrow.h"
#include "MantidQtWidgets/Common/Batch/CellDelegate.h"
#include "MantidQtWidgets/Common/Batch/ExtractSubtrees.h"
#include "MantidQtWidgets/Common/Batch/FindSubtreeRoots.h"
#include "MantidQtWidgets/Common/Batch/QtTreeCursorNavigation.h"
#include <QKeyEvent>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <algorithm>
#include <iostream>
namespace MantidQt {
namespace MantidWidgets {
namespace Batch {

JobTreeView::JobTreeView(QStringList const &columnHeadings, QWidget *parent)
    : QTreeView(parent), m_model(this) {
  setModel(&m_model);
  setHeaderLabels(columnHeadings);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setItemDelegate(new CellDelegate(this, *this));
}

void JobTreeView::commitData(QWidget *editor) {
  QTreeView::commitData(editor);

  auto location = rowLocationAt(m_lastEdited);
  auto *const cell = adaptedModel().modelItemFromIndex(m_lastEdited);
  auto cellText = cell->text().toStdString();

  m_notifyee->notifyCellChanged(rowLocationAt(m_lastEdited),
                                m_lastEdited.column(), cellText);
  m_hasEditorOpen = false;
}

bool JobTreeView::edit(const QModelIndex &index, EditTrigger trigger,
                       QEvent *event) {
  m_lastEdited = index;
  m_hasEditorOpen = true;
  return QTreeView::edit(index, trigger, event);
}

std::vector<RowLocation> JobTreeView::selectedRowLocations() const {
  auto selection = selectionModel()->selectedRows();
  std::vector<RowLocation> rowSelection;
  rowSelection.reserve(selection.size());
  std::transform(selection.begin(), selection.end(),
                 std::back_inserter(rowSelection),
                 [&](QModelIndex const &index) -> RowLocation {
                   return rowLocationAt(index);
                 });
  return rowSelection;
}

void JobTreeView::removeSelectedRequested() {
  m_notifyee->notifyRemoveRowsRequested(selectedRowLocations());
}

void JobTreeView::copySelectedRequested() {
  m_notifyee->notifyCopyRowsRequested(selectedRowLocations());
}

void JobTreeView::pasteSelectedRequested() {
  m_notifyee->notifyPasteRowsRequested(selectedRowLocations());
}

void JobTreeView::removeRows(std::vector<RowLocation> rowsToRemove) {
  std::sort(rowsToRemove.begin(), rowsToRemove.end());

  std::cout << "\n\n";
  for (auto &&loc : rowsToRemove)
    std::cout << loc << '\n';
  std::cout << std::endl;

  for (auto rowIt = rowsToRemove.crbegin(); rowIt < rowsToRemove.crend();
       ++rowIt)
    removeRowAt(*rowIt);
}

std::string JobTreeView::textAt(RowLocation location, int column) const {
  auto const cellIndex = modelIndexAt(location, column);
  return adaptedModel().textFromCell(cellIndex);
}

void JobTreeView::setTextAt(RowLocation location, int column,
                            std::string const &cellText) {
  auto const cellIndex = modelIndexAt(location, column);
  adaptedModel().setTextAtCell(cellIndex, cellText);
}

void JobTreeView::setRowTextAt(RowLocation const &location,
                               std::vector<std::string> const &rowText) {}

std::vector<std::string>
JobTreeView::rowTextAt(RowLocation const &location) const {
  return adaptedModel().rowTextFromRow(modelIndexAt(location));
}

void JobTreeView::replaceSubtree(
    RowLocation const &rootToRemove,
    typename ExtractSubtrees::Subtree const &toInsert) {
  auto const insertionParent = rootToRemove.parent();
  auto insertionIndex = rootToRemove.rowRelativeToParent();
  removeRowAt(rootToRemove);
  insertSubtreeAt(insertionParent, insertionIndex, toInsert);
}

typename ExtractSubtrees::Subtree::const_iterator
JobTreeView::insertSubtreeRecursive(
    RowLocation const &parent, int depth,
    typename ExtractSubtrees::Subtree::const_iterator current,
    typename ExtractSubtrees::Subtree::const_iterator end) {

  while (current != end) {
    auto currentRow = (*current).first;
    auto currentDepth = currentRow.depth();
    if (depth < currentDepth) {
      current =
          insertSubtreeRecursive(parent.child(currentRow.rowRelativeToParent()),
                                 depth + 1, current, end); // HACK?
    } else if (depth > currentDepth) {
      return current;
    } else {
      appendChildRowOf(parent, (*current).second);
      ++current;
    }
  }
  return end;
}

void JobTreeView::insertSubtreeAt(
    RowLocation const &parent, int index,
    typename ExtractSubtrees::Subtree const &subtree) {
  insertChildRowOf(parent, index, subtree[0].second);

  auto insertionParent = parent.child(index);
  auto lastDepth = subtree[0].first.depth();
  insertSubtreeRecursive(insertionParent, lastDepth, subtree.cbegin() + 1,
                         subtree.cend());
}

void JobTreeView::replaceRows(
    std::vector<RowLocation> toRemove, std::vector<RowLocation> toInsert,
    std::vector<std::vector<std::string>> textToInsert) {
  try {

    auto findSubtreeRoots = FindSubtreeRoots();

    std::sort(toRemove.begin(), toRemove.end());
    auto subtreesToRemove = findSubtreeRoots(toRemove).value();

    auto selectionBase = [&]() -> QModelIndex {
      if (toRemove.empty()) {
        return adaptedModel().rootModelIndex();
      } else {
        auto firstSelectionRoot = *toRemove.begin();
        return modelIndexAt(subtreesToRemove[0]).parent();
      }
    }();

    // TODO Differentiate between nodes with no children and nodes which have
    // children?

    auto extractSubtrees = ExtractSubtrees();
    std::sort(toInsert.begin(), toInsert.end());
    auto subtreesToInsert = extractSubtrees(toInsert, textToInsert).value();

    auto subtreeToRemove = subtreesToRemove.cbegin();
    auto subtreeToInsert = subtreesToInsert.cbegin();

    for (; subtreeToRemove != subtreesToRemove.cend() &&
           subtreeToInsert != subtreesToInsert.cend();
         ++subtreeToRemove, ++subtreeToInsert) {
      replaceSubtree(/*toRemove=*/*subtreeToRemove,
                     /*toInsert=*/*subtreeToInsert);
    }

    if (subtreesToRemove.size() > subtreesToInsert.size()) {
      for (; subtreeToRemove != subtreesToRemove.cend(); ++subtreeToRemove)
        removeRowAt(*subtreeToRemove);
    } else if (subtreesToRemove.size() < subtreesToInsert.size()) {
      for (auto &toInsert = *(subtreeToInsert);
           subtreeToInsert != subtreesToInsert.cend(); ++subtreeToInsert) {
        // appendSubtreeAt(toRemove.back().parent(), toInsert);
      }
    }
  } catch(std::exception& ex) {
    std::cout << ex.what() << std::endl;
  }
}

void JobTreeView::setHeaderLabels(QStringList const &columnHeadings) {
  m_model.setHorizontalHeaderLabels(columnHeadings);

  for (auto i = 0; i < model()->columnCount(); ++i)
    resizeColumnToContents(i);
}

void JobTreeView::subscribe(JobTreeViewSubscriber &subscriber) {
  m_notifyee = &subscriber;
}

boost::optional<QModelIndex>
JobTreeView::modelIndexIfExistsAt(RowLocation const &location,
                                  int column) const {
  auto parentIndex = adaptedModel().rootModelIndex();
  if (location.isRoot()) {
    return parentIndex;
  } else {
    auto &path = location.path();
    for (auto it = path.cbegin(); it != path.cend() - 1; ++it)
      parentIndex = model()->index(*it, column, parentIndex);
    if (model()->hasIndex(location.rowRelativeToParent(), column, parentIndex))
      return model()->index(location.rowRelativeToParent(), column,
                            parentIndex);
    else
      return boost::none;
  }
}

QModelIndex JobTreeView::modelIndexAt(RowLocation const &location,
                                      int column) const {
  auto maybeIndex = modelIndexIfExistsAt(location, column);
  if (maybeIndex.is_initialized())
    return maybeIndex.get();
  else
    throw std::runtime_error("modelIndexAt: Attempted to get model index for "
                             "row location which does not exist.");
}

RowLocation JobTreeView::rowLocationAt(QModelIndex const &index) const {
  if (index.isValid()) {
    auto pathComponents = RowPath();
    auto currentIndex = index;
    while (currentIndex.isValid()) {
      pathComponents.insert(pathComponents.begin(), currentIndex.row());
      currentIndex = currentIndex.parent();
    }
    return RowLocation(pathComponents);
  } else {
    return RowLocation();
  }
}

bool JobTreeView::indexesAreOnSameRow(QModelIndex const &a,
                                      QModelIndex const &b) const {
  return a.parent() == b.parent() && a.row() == b.row() &&
         a.model() == b.model();
}

bool JobTreeView::hasEditorOpen() const { return m_lastEdited.isValid(); }

bool JobTreeView::rowRemovalWouldBeIneffective(
    QModelIndex const &indexToRemove) const {
  return indexesAreOnSameRow(currentIndex(), indexToRemove) && hasEditorOpen();
}

QModelIndex
JobTreeView::siblingIfExistsElseParent(QModelIndex const &index) const {
  if (hasRowAbove(index)) {
    return above(index);
  } else if (hasRowBelow(index)) {
    return below(index);
  } else {
    return index.parent();
  }
}

bool JobTreeView::isOnlyChild(QModelIndex const &index) const {
  auto parentIndex = model()->parent(index);
  return model()->rowCount(parentIndex) == 1;
}

bool JobTreeView::isOnlyChildOfRoot(QModelIndex const &index) const {
  return !model()->parent(index).isValid() && isOnlyChild(index);
}

void JobTreeView::removeRowAt(RowLocation const &location) {
  auto indexToRemove = modelIndexAt(location);
  assertOrThrow(indexToRemove.isValid(),
                "removeRowAt: Attempted to remove the invisible root item.");
  assertOrThrow(!isOnlyChildOfRoot(indexToRemove),
                "Attempted to delete the only child of the invisible root "
                "item. Try removeAllRows() instead.");
  if (rowRemovalWouldBeIneffective(indexToRemove)) {
    auto rowIndexToSwitchTo = siblingIfExistsElseParent(indexToRemove);
    setCurrentIndex(rowIndexToSwitchTo);
  }
  adaptedModel().removeRowAt(indexToRemove);
}

void JobTreeView::insertChildRowOf(RowLocation const &parent, int beforeRow) {
  adaptedModel().insertEmptyChildRow(modelIndexAt(parent), beforeRow);
}

void JobTreeView::insertChildRowOf(RowLocation const &parent, int beforeRow,
                                   std::vector<std::string> const &rowText) {
  adaptedModel().insertChildRow(modelIndexAt(parent), beforeRow,
                                adaptedModel().rowFromRowText(rowText));
}

void JobTreeView::appendChildRowOf(RowLocation const &parent) {
  adaptedModel().appendEmptyChildRow(modelIndexAt(parent));
}

void JobTreeView::appendChildRowOf(RowLocation const &parent,
                                   std::vector<std::string> const &rowText) {
  adaptedModel().appendChildRow(modelIndexAt(parent),
                                adaptedModel().rowFromRowText(rowText));
}

void JobTreeView::editAt(QModelIndex const &index) {
  clearSelection();
  setCurrentIndex(index);
  edit(index);
}

QModelIndex JobTreeView::expanded(QModelIndex const &index) {
  auto expandAt = index;
  while (expandAt.isValid()) {
    setExpanded(expandAt, true);
    expandAt = model()->parent(expandAt);
  }
  return index;
}

QtStandardItemMutableTreeAdapter JobTreeView::adaptedModel() {
  return QtStandardItemMutableTreeAdapter(m_model);
}

QtStandardItemTreeAdapter const JobTreeView::adaptedModel() const {
  return QtStandardItemTreeAdapter(m_model);
}

QtTreeCursorNavigation JobTreeView::navigation() const {
  return QtTreeCursorNavigation(model());
}

std::pair<QModelIndex, bool>
JobTreeView::findOrMakeCellBelow(QModelIndex const &index) {
  if (hasRowBelow(index))
    return std::make_pair(below(index), false);
  else
    return std::make_pair(adaptedModel().appendEmptySiblingRow(index), true);
}

void JobTreeView::appendAndEditAtChildRow() {
  auto const parent = currentIndex();
  auto const child = adaptedModel().appendEmptyChildRow(parent);
  editAt(expanded(child));
  m_notifyee->notifyRowInserted(rowLocationAt(child));
}

void JobTreeView::appendAndEditAtRowBelow() {
  auto const below = findOrMakeCellBelow(currentIndex());
  auto index = below.first;
  auto isNew = below.second;
  editAt(index);
  if (isNew)
    m_notifyee->notifyRowInserted(rowLocationAt(index));
}

void JobTreeView::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Return) {
    if (event->modifiers() & Qt::ControlModifier) {
      appendAndEditAtChildRow();
    } else if (event->modifiers() & Qt::ShiftModifier) {
      // TODO Go to row above
      // prependAndEditAtRowAbove();
    } else {
      appendAndEditAtRowBelow();
    }
  } else if (event->key() == Qt::Key_Delete) {
    removeSelectedRequested();
  } else if (event->key() == Qt::Key_C) {
    if (event->modifiers() & Qt::ControlModifier) {
      copySelectedRequested();
    }
  } else if (event->key() == Qt::Key_V) {
    if (event->modifiers() & Qt::ControlModifier) {
      pasteSelectedRequested();
    }
  } else {
    QTreeView::keyPressEvent(event);
  }
}

QModelIndex
JobTreeView::applyNavigationResult(QtTreeCursorNavigationResult const &result) {
  auto shouldMakeNewRowBelow = result.first;
  if (shouldMakeNewRowBelow) {
    auto newIndex = adaptedModel().appendEmptySiblingRow(result.second);
    auto newRowIndex = firstCellOnRowOf(newIndex);
    auto newRowLocation = rowLocationAt(newRowIndex);

    m_notifyee->notifyRowInserted(newRowLocation);

    return expanded(
        modelIndexIfExistsAt(newRowLocation).get_value_or(QModelIndex()));
  } else {
    return result.second;
  }
}

QModelIndex JobTreeView::moveCursor(CursorAction cursorAction,
                                    Qt::KeyboardModifiers modifiers) {
  if (cursorAction == QAbstractItemView::MoveNext) {
    return applyNavigationResult(navigation().moveCursorNext(currentIndex()));
  } else if (cursorAction == QAbstractItemView::MovePrevious) {
    return navigation().moveCursorPrevious(currentIndex());
  } else {
    return QTreeView::moveCursor(cursorAction, modifiers);
  }
}

} // namespace Batch
} // namespace MantidWidgets
} // namespace MantidQt