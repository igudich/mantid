"""Test suite for the crystal field calculations in the Inelastic/CrystalField package
"""
import unittest
import numpy as np

# Import mantid to setup the python paths to the bundled scripts
import mantid
from CrystalField.energies import energies


class CrystalFieldTests(unittest.TestCase):

    def _do_test_eigensystem(self, en, wf, ham):
        n = len(en)
        wf_ctr = np.conj(wf.transpose())
        I = np.tensordot(wf_ctr, wf, axes=1)
        for i in range(n):
            re = np.real(I[i, i])
            im = np.imag(I[i, i])
            self.assertAlmostEqual(re, 1.0, 10)
            self.assertAlmostEqual(im, 0.0, 10)
        tmp = np.tensordot(wf_ctr, ham, axes=1)
        E = np.tensordot(tmp, wf, axes=1)
        ev = np.real([E[i, i] for i in range(n)])
        emin = np.amin(ev)
        for i in range(n):
            for j in range(n):
                if i == j:
                    self.assertAlmostEqual(np.real(E[i, j] - emin), en[i], 10)
                    self.assertAlmostEqual(np.imag(E[i, j]), 0.0, 10)
                else:
                    self.assertAlmostEqual(np.real(E[i, j]), 0.0, 10)
                    self.assertAlmostEqual(np.imag(E[i, j]), 0.0, 10)

    def xtest_C2v(self):
        en, wf, ham = energies(1, B20=0.3365, B22=7.4851, B40=0.4062, B42=-3.8296, B44=-2.3210)
        self._do_test_eigensystem(en, wf, ham)

    def xtest_C2(self):
        en, wf, ham = energies(1, B20=0.3365, B22=7.4851, B40=0.4062, IB42=-3.8296, IB44=-2.3210)
        self._do_test_eigensystem(en, wf, ham)

    def xtest_C2v_mol(self):
        en, wf, ham = energies(1, B20=0.3365, B22=7.4851, B40=0.4062, B42=-3.8296, B44=-2.3210,
                               BmolX=1.0, BmolY=2.0, BmolZ=3.0)
        self._do_test_eigensystem(en, wf, ham)

    def xtest_C2v_ext(self):
        en, wf, ham = energies(1, B20=0.3365, B22=7.4851, B40=0.4062, B42=-3.8296, B44=-2.3210,
                               BextX=1.0, BextY=2.0, BextZ=3.0)
        self._do_test_eigensystem(en, wf, ham)

    def xtest_upd3(self):
        # Parameters are from Phys Rev B 89 235114 / arXiv:1403.4785, originally calculated using McPhase
        # Ion is U4+ which is equivalent to Pr3+ (5f2 instead of 4f2)
        en, wf, ham = energies(2, B20=0.035, B40=-0.012, B43=-0.027, B60=-0.00012, B63=0.0025, B66=0.0068)
        self._do_test_eigensystem(en, wf, ham)
        expectedEigenvalues = [0, 0, 4.36, 9.64, 9.64, 20.65, 30.03, 30.03, 52.50]
        emin = np.amin(en)
        for i in range(9):
            self.assertAlmostEqual(expectedEigenvalues[i], en[i], 1)
        # Now test the eigenvectors by computing the dipole transition matrix elements. Use the magnetic field
        #   terms but divide by gJ*uB (gJ=0.8 for U4+/Pr3+ and uB=0.6715)
        _, _, hx = energies(2, BextX=1.0)
        _, _, hy = energies(2, BextY=1.0)
        _, _, hz = energies(2, BextZ=1.0)
        ix = np.dot(np.conj(np.transpose(wf)), np.dot(hx, wf))
        iy = np.dot(np.conj(np.transpose(wf)), np.dot(hy, wf))
        iz = np.dot(np.conj(np.transpose(wf)), np.dot(hz, wf))
        gJuB = 0.53716
        trans = np.multiply(ix, np.conj(ix)) + np.multiply(iy, np.conj(iy)) + np.multiply(iz, np.conj(iz))
        # For some reason, in the paper I also divided the matrix elements by a factor of 4. (not sure why)
        trans = trans / (gJuB ** 2) / 4
        expectedDipoleTM = [0, 0, 0.31, 1.24 / 2., 1.24 / 2., 2.04, 3.4 / 2., 3.4 / 2., 0]
        for i in range(2, 9):
            self.assertAlmostEqual(expectedDipoleTM[i], trans[i, 0] + trans[i, 1], 1)

    def test_api_CrystalField(self):
        from CrystalField import CrystalField
        cf = CrystalField('Pr', 'C2v', B20=0.035, B40=-0.012, B43=-0.027, B60=-0.00012, B63=0.0025, B66=0.0068)
        def set_ion(x):
            cf.Ion = x
        self.assertRaises(RuntimeError, set_ion, 'He')
        def set_symmetry(x):
            cf.Symmetry = x
        self.assertRaises(RuntimeError, set_symmetry, 'G')

    def test_api_CrystalField_eigensystem(self):
        from CrystalField import CrystalField
        cf = CrystalField('Ce', 'C2v', B20=0.035, B40=-0.012, B43=-0.027, B60=-0.00012, B63=0.0025, B66=0.0068)

        cf._calcEigensystem()
        ev = cf._eigenvalues
        self.assertAlmostEqual(ev[0], 0.0, 10)
        self.assertAlmostEqual(ev[1], 0.0, 10)
        self.assertAlmostEqual(ev[2], 1.44393213, 8)
        self.assertAlmostEqual(ev[3], 1.44393213, 8)
        self.assertAlmostEqual(ev[4], 3.85696607, 8)
        self.assertAlmostEqual(ev[5], 3.85696607, 8)

if __name__ == "__main__":
    unittest.main()
