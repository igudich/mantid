# Copyright &copy; 2014,2015 ISIS Rutherford Appleton Laboratory, NScD
# Oak Ridge National Laboratory & European Spallation Source
#
# This file is part of Mantid.
# Mantid is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Mantid is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# File change history is stored at: <https://github.com/mantidproject/mantid>.
# Code Documentation is available at: <http://doxygen.mantidproject.org>

import os
import time
import numpy as np

# For 3d median filter
try:
    import scipy
except ImportError:
    raise ImportError("Could not find the package scipy which is required for image pre-/post-processing")

try:
    import scipy.ndimage
except ImportError:
    raise ImportError("Could not find the subpackage scipy.ndimage, required for image pre-/post-processing")

import tomorec.io as tomoio

#pylint: disable=too-few-public-methods
class ToolAlgorithmConfig(object):
    """
    Reconstruction algorithm specific configuration. Required for any reconstruction:
    a tool that implements the method/algorithm,
    the algorithm name,
    (only for the iterative algorithms) the number of iterations
    (only for some algorithms) a regularization/smoothing parameter
    """

    DEF_TOOL = 'tomopy'
    DEF_ALGORITHM = 'gridrec'

    def __init__(self):
        self.tool = self.DEF_TOOL
        self.algorithm = self.DEF_ALGORITHM
        self.num_iter = None
        self.regularization = None

    def __str__(self):
        mystr = "Tool: {0}\n".format(self.tool)
        mystr += "Algorithm: {0}\n".format(self.algorithm)
        if self.num_iter:
            mystr += "Number of algorith iterations: {0}\n".format(self.num_iter)
        else:
            mystr += "(Algorithm iterations: not defined)\n"
        if self.regularization:
            mystr += "Regularization parameter: {0}\n".format(self.regularization)
        else:
            mystr += "(Regularization parameter: not defined)\n"

        return mystr


#pylint: disable=too-many-instance-attributes
class PreProcConfig(object):
    """
    All pre-processing options required to run a tomographic reconstruction.

    Options like the stripe removal, MCP correction, or even the median filter would
    better be handled as plugins. For the time being we just have a panel with a fixed
    set of options to enable/disable/configure
    """

    DEF_NUM_ITER = 5

    def __init__(self):
        # defaults that look sensible for the MCP detector:
        # median_filter=3, rotate=-1, crop=[0,  252, 0, 512], MCP correction: on
        self.input_dir = None
        self.in_img_format = 'tiff'
        self.out_img_format = 'tiff'
        self.max_angle = 360
        # Center of rotation
        self.cor = None
        self.normalize_flat_dark = True
        self.normalize_proton_charge = False
        # region of interest
        self.crop_coords = None
        self.cut_off_level = 0
        self.mcp_correction = True
        self.scale_down = 0
        self.median_filter_size = 3
        # Rotation 90 degrees clockwise (positive) or counterclockwise (negative)
        # Example: -1 => (-90 degrees == 90 degrees counterclockwise)
        self.rotation = -1
        self.line_projection = True
        self.stripe_removal_method = 'wavelet-fourier'
        self.save_preproc_imgs = True

    def __str__(self):

        mystr = "Input path (relative): {0}\n".format(self.input_dir)
        mystr += "Input path (absolute): {0}\n".format(os.path.abspath(self.input_dir))
        mystr += "Input image format: {0}\n".format(self.in_img_format)
        mystr += "Output image format: {0}\n".format(self.out_img_format)
        mystr += "Maximum angle:: {0}\n".format(self.max_angle)
        mystr += "Center of rotation: {0}\n".format(self.cor)
        mystr += "Region of interest (crop coordinates): {0}\n".format(self.crop_coords)
        mystr += "Normalize by flat/dark images: {0}\n".format(self.normalize_flat_dark)
        mystr += "Normalize by proton charge: {0}\n".format(self.normalize_proton_charge)
        mystr += "Cut-off on normalized images: {0}\n".format(self.cut_off_level)
        mystr += "Corrections for MCP detector: {0}\n".format(self.mcp_correction)
        mystr += "Scale down factor for images: {0}\n".format(self.scale_down)
        mystr += "Median filter width: {0}\n".format(self.median_filter_size)
        mystr += "Rotation: {0}\n".format(self.rotation)
        mystr += "Line projection (line integral/log re-scale): {0}\n".format(1)
        mystr += "Sinogram stripes removal: {0}".format(self.stripe_removal_method)

        return mystr


class PostProcConfig(object):
    """
    All pre-processing options required to run a tomographic reconstruction
    """

    def __init__(self):
        """
        Builds a default post-processing configuration with a sensible choice of parameters
        """
        self.output_dir = None
        self.circular_mask = 0.94
        self.cut_off_level = 0
        self.gaussian_filter_par = 0
        self.median_filter_size = 0
        self.median_filter3d_size = 0

    def __str__(self):

        mystr = "Output path (relative): {0}\n".format(self.output_dir)
        mystr += "Output path (absolute): {0}\n".format(os.path.abspath(self.output_dir))
        mystr += "Circular mask: {0}\n".format(self.circular_mask)
        mystr += "Cut-off on reconstructed volume: {0}\n".format(self.cut_off_level)
        mystr += "Gaussian filter: {0}\n".format(self.gaussian_filter_par)
        mystr += "Median filter size:: {0}\n".format(self.median_filter_size)
        mystr += "Median filter (3d) size:: {0}\n".format(self.median_filter3d_size)

        return mystr

class ReconstructionConfig(object):
    """
    Full configuration (pre-proc + tool/algorithm + post-proc.
    """

    def __init__(self, preproc_cfg, alg_cfg, postproc_cfg):
        self.preproc_cfg = preproc_cfg
        self.alg_cfg = alg_cfg
        self.postproc_cfg = postproc_cfg

    def __str__(self):
        return str(self.preproc_cfg) + str(self.alg_cfg) + str(self.postproc_cfg)


class ReconstructionCommand(object):
    """
    Run a tomographic reconstruction command, which can be a local process or a job on a
    remote machine/scheduler.
    """

    def __init__(self):
        self._OUT_README_FNAME = '0.README_reconstruction.txt'
        self.preproc_cfg = PreProcConfig()
        self.alg_cfg = ToolAlgorithmConfig
        self.postproc_cfg = PostProcConfig()

    def do_recon(self, cfg, cmd_line=None):
        """
        Run a reconstruction using a particular tool, algorithm and setup

        @param preproc_cfg :: configuration (pre-processing + tool+algorithm + post-processing)

        @param cmd_line :: command line text if running from the CLI. When provided it will
        be written in the output readme file(s) for reference.
        """

        readme_fullpath = os.path.join(cfg.postproc_cfg.output_dir, self._OUT_README_FNAME)
        tstart = self.gen_readme_summary_begin(readme_fullpath, cfg, cmd_line)

        data, white, dark = self.read_in_stack(cfg.preproc_cfg.input_dir, cfg.preproc_cfg.in_img_format)
        print "Shape of raw data: {0}, dtype: {1}".format(data.shape, data.dtype)

        # These imports will raise appropriate exceptions in case of error
        import tomorec.tool_imports as tti
        if 'astra' == cfg.alg_cfg.tool:
            tti.import_tomo_tool('astra')
        elif 'tomopy' == cfg.alg_cfg.tool:
            tti.import_tomo_tool('tomopy')

        preproc_data = self.apply_all_preproc(data, cfg.preproc_cfg, white, dark)
        print "Shape of pre-processed data: {0}".format(preproc_data.shape)

        # Save pre-proc images
        self.save_preproc_images(cfg.postproc_cfg.output_dir, preproc_data, cfg.preproc_cfg)

        # Reconstruction
        t_recon_start = time.time()
        recon_data = self.run_reconstruct_3d(preproc_data, cfg.preproc_cfg, cfg.alg_cfg)
        t_recon_end = time.time()
        print("Reconstructed volume. Shape: {0}, and pixel data type: {1}".
              format(recon_data.shape, recon_data.dtype))

        # Post-processing
        self.apply_postproc_filters(recon_data, cfg.postproc_cfg)

        # Save output from the reconstruction
        self.save_recon_output(recon_data, cfg.postproc_cfg.output_dir, save_netcdf_vol=True)

        self.gen_readme_summary_end(readme_fullpath, (data, preproc_data, recon_data), tstart,
                                    t_recon_end - t_recon_start)

    def apply_all_preproc(self, data, preproc_cfg, white, dark):
        """
        Do all the pre-processing. This does all that is needed between a)
        loading the input data, and b) starting a reconstruction
        run/job. From raw inputs to pre-proc data that is ready to go for
        reconstruction.

        @param data :: raw data (sample projection images)
        @param preproc_cfg :: pre-processing configuration
        @param white :: white / flat / open-beam image for normalization in some of the first
        pre-processing steps
        @param dark :: dark image for normalization

        Returns :: pre-processed data.

        """
        preproc_data = self.apply_prep_filters(data, preproc_cfg, white, dark)
        preproc_data = self.apply_line_projection(preproc_data, preproc_cfg)
        preproc_data = self.apply_final_preproc_corrections(preproc_data, preproc_cfg)

        return preproc_data

    def gen_readme_summary_begin(self, filename, cfg, cmd_line):
        """
        To write configuration, settings, etc. early on. As early as possible, before any failure
        can happen.

        @param filename :: name of the readme/final report file
        @param cfg :: full reconstruction configuration
        @param cmd_line :: command line originally used to run this reconstruction, when running
        from the command line

        Returns :: time now (begin of run) in number of seconds since epoch (time() time)
        """
        tstart = time.time()

        # generate file with dos/windows line end for windoze users' convenience
        with open(filename, 'w') as oreadme:
            file_hdr = ('Tomographic reconstruction. Summary of inputs, settings and outputs.\n'
                        'Time now (run begin): ' + time.ctime(tstart) +
                        '\n')
            oreadme.write(file_hdr)

            alg_hdr = ("\n"
                       "--------------------------\n"
                       "Tool/Algorithm\n"
                       "--------------------------\n")
            oreadme.write(alg_hdr)
            oreadme.write(str(cfg.alg_cfg))
            oreadme.write("\n")

            preproc_hdr = ("\n"
                           "--------------------------\n"
                           "Pre-processing parameters\n"
                           "--------------------------\n")
            oreadme.write(preproc_hdr)
            oreadme.write(str(cfg.preproc_cfg))
            oreadme.write("\n")

            postproc_hdr = ("\n"
                            "--------------------------\n"
                            "Post-processing parameters\n"
                            "--------------------------\n")
            oreadme.write(postproc_hdr)
            oreadme.write(str(cfg.postproc_cfg))
            oreadme.write("\n")

            cmd_hdr = ("\n"
                       "--------------------------\n"
                       "Command line\n"
                       "--------------------------\n")
            oreadme.write(cmd_hdr)
            oreadme.write(cmd_line)
            oreadme.write("\n")

        return tstart

    def gen_readme_summary_end(self, filename, data_stages, tstart, t_recon_elapsed):
        """
        Write last part of report in the output readme/report file. This should be used whenever a
        reconstruction runs correctly.

        @param filename :: name of the readme/final report file
        @param data_stages :: tuple with data in three stages (raw, pre-processed, reconstructed)
        @param tstart :: time at the beginning of the job/reconstruction, when the first part of the
        readme file was written
        @param t_recon_elapsed :: reconstruction time
        """
        # append to a readme/report that should have been pre-filled with the initial configuration
        with open(filename, 'a') as oreadme:

            run_hdr = ("\n"
                       "--------------------------\n"
                       "Run/job details:\n"
                       "--------------------------\n")
            oreadme.write(run_hdr)
            (raw_data, preproc_data, recon_data) = data_stages

            oreadme.write("Dimensions of raw input sample data: {0}\n".format(raw_data.shape))
            oreadme.write("Dimensions of pre-processed sample data: {0}\n".format(preproc_data.shape))
            oreadme.write("Dimensions of reconstructed volume: {0}\n".format(recon_data.shape))

            oreadme.write("Raw input pixel type: {0}\n".format(raw_data.dtype))
            oreadme.write("Output pixel type: {0}\n".format('uint16'))
            oreadme.write("Time elapsed in reconstruction: {0:.3f}s\r\n".format(t_recon_elapsed))
            tend = time.time()
            oreadme.write("Total time elapsed: {0:.3f}s\r\n".format(tend-tstart))
            oreadme.write('Time now (run end): ' + time.ctime(tend))

    def normalize_proton_charge(self, data, pre_cfg):
        """
        """
        if pre_cfg.normalize_proton_charge:
            air_sums = []
            for idx in range(0, data.shape[0]):
                air_data_sum = data[idx, cfg.crop_coords[1]:cfg.crop_coords[3],
                                    cfg.crop_coords[0]:cfg.crop_coords[2]].sum()
                air_sums.append(air_data_sum)

            too_verbose = False
            if too_verbose:
                print "air sums: ", air_sums
            avg = np.average(air_sums)
            if too_verbose:
                print("Air avg: {0}, max ratio: {1}, min ratio: {2}".format(
                    avg, np.max(air_sums)/avg, np.min(air_sums)/avg))

        return data

    def crop_coords(self, data, cfg):
        """
        """
        # list with first-x, first-y, second-x, second-y
        if cfg.crop_coords:
            if  isinstance(cfg.crop_coords, list) and 4 == len(cfg.crop_coords):
                data = data[:, cfg.crop_coords[1]:cfg.crop_coords[3], cfg.crop_coords[0]:cfg.crop_coords[2]]
            else:
                print("Error in crop (region of interest) parameter (expecting a list with four integers. "
                      "Got: {0}.".format(crop))

            print " * Finished crop step, with pixel data type: {0}.".format(data.dtype)
        else:
            print " * Note: not applying crop."

        return data

    def normalize_flat_dark(self, data, cfg):
        """
        """
        if False and cfg.normalize_flat_dark:
            norm_divide = None
            if norm_dark_img:
                norm_divide = norm_flat_img - norm_dark_img
            if cfg.crop_coords:
                norm_divide = norm_divide[:, cfg.crop_coords[1]:cfg.crop_coords[3],
                                          cfg.crop_coords[0]:cfg.crop_coords[2]]
            # prevent divide-by-zero issues
            norm_divide[norm_divide==0] = 1e-6

            for idx in range(0, data.shape[0]):
                data[idx, :, :] = np.true_divide(data[idx, :, :] - norm_dark_img, norm_divide)
            # true_divide produces float64, we assume that precision not needed (definitely not
            # for 16-bit depth images as we usually have.
            print " * Finished normalization by flat/dark images with pixel data type: {0}.".format(data.dtype)
        else:
            print " * Note: not applying normalization by flat/dark images."

        return data

    def apply_cut_off(self, data, cfg):
        # Apply cut-off for the normalization?
        if cfg.cut_off_level and cfg.cut_off_level:
            print "* Applying cut-off with level: {0}".format(cfg.cut_off_level)
            dmin = np.amin(data)
            dmax = np.amax(data)
            rel_cut_off = dmin + cfg.cut_off_level * (dmax - dmin)
            data[data < rel_cut_off] = dmin
            print " * Finished cut-off stepa, with pixel data type: {0}".format(data.dtype)
        else:
            print " * Note: not applying cut-off."

        if cfg.mcp_corrections:
            print " * MCP corrections not implemented in this version"

        if cfg.scale_down:
            print " * Scale down not implemented in this version"

        if cfg.median_filter_size and cfg.median_filter_size > 1:
            for idx in range(0, data.shape[0]):
                data[idx] = scipy.ndimage.median_filter(data[idx], cfg.median_filter_size, mode='mirror')
                #, mode='nearest')
            print " * Finished noise filter / median, with pixel data type: {0}.".format(data.dtype)
        else:
            print " * Note: not applying noise filter /median."

        return data

    def rotate_imgs(self, data, cfg):
        """
        @param data :: image stack as a 3d numpy array
        @param cfg :: pre-processing configuration
        """
        data_rotated = data
        if cfg.rotation:
            data_rotated = np.zeros((data.shape[0], data.shape[2], data.shape[1]), dtype=data.dtype)
            for idx in range(0, data.shape[0]):
                data_rotated[idx] = np.rot90(data[idx,:,:], cfg.rotation)
            print " * Finished rotation step, with pixel data type: {0}".format(data_rotated.dtype)

        return data_rotated

    def apply_prep_filters(self, data, cfg, white, dark):
        """
        Apply the normal initial pre-processing filters, including simple
        operations as selecting/cropping to the region-of-interest,
        normalization, etc.

        @param data :: projection images data, as 3d numpy array, with images along outermost (z)
        dimension

        @param cfg :: pre-processing configuration
        @param white :: white/flat/open-beam image for normalization
        @param dark :: dark image for normalization

        Returns :: process/filtered data (sizes can change (cropped) and data can be rotated)
        """
        print " * Beginning pre-processing with pixel data type:", data.dtype
        if 'float64' == data.dtype:
            data = data.astype(dtype='float32')
            print " * Note: pixel data type changed to:", data.dtype

        data = self.crop_coords(data, cfg)
        data = self.normalize_proton_charge(data, cfg)
        data = self.normalize_flat_dark(data, cfg)

        data = self.apply_cut_off(data, cfg)
        data = self.rotate_imgs(data, cfg)

        return data

    def apply_line_projection(self, imgs_angles, preproc_cfg):
        """
        Transform pixel values as $- ln (Is/I0)$, where $Is$ is the pixel (intensity) value and $I0$ is a
        reference value (pixel/intensity value for open beam, or maximum in the stack or the image).

        This produces a projection image, $ p(s) = -ln\\frac{I(s)}{I(0)} $,
        with $ I(s) = I(0) e^{-\\int_0^s \\mu(x)dx} $
        where:
        $p(s)$ represents the sum of the density of objects along a line (pixel) of the beam
        I(0) initital intensity of netron beam (white images)
        I(s) neutron count measured by detector/camera

        The integral is the density along the path through objects.
        This is required for example when pixels have neutron count values.

        @param imgs_angles :: stack of images (angular projections) as 3d numpy array. Every image will be
        processed independently, using as reference intensity the maximum pixel value found across all the
        images.

        @param preproc_cfg :: pre-processing configuration set up for a reconstruction

        Returns :: projected data volume (image stack)
        """
        if not preproc_cfg.line_projection:
            return imgs_angles

        imgs_angles = imgs_angles.astype('float32')
        for idx in range(0, imgs_angles.shape[0]):
            max_img = np.amax(imgs_angles[idx, :, :])
            to_log = np.true_divide(imgs_angles[idx, :, :], max_img)
            if False:
                print("initial img max: {0}. transformed to log scale,  min: {1}, max: {2}".
                      format(max_img, np.amin(to_log), np.amax(to_log)))
            imgs_angles[idx, :, :] = - np.log(to_log +1e-6)

        return imgs_angles

    def apply_final_preproc_corrections(self, preproc_data, cfg):
        """
        Apply additional, optional, pre-processing steps/filters.

        @param preproc_data :: input data as a 3d volume (projection images)
        @param cfg :: pre-processing configuration
        """
        # Remove stripes in sinograms / ring artefacts in reconstructed volume
        if cfg.stripe_removal_method:
            import prep as iprep
            if 'wavelet-fourier' == cfg.stripe_removal_method.lower():
                time1 = time.time()
                print " * Removing stripes/ring artifacts using the method '{0}'".format(cfg.stripe_removal_method)
                #preproc_data = tomopy.prep.stripe.remove_stripe_fw(preproc_data)
                preproc_data = iprep.filters.remove_stripes_ring_artifacts(preproc_data, 'wavelet-fourier')
                time2 = time.time()
                print " * Removed stripes/ring artifacts. Time elapsed: {0:.3f}".format(time2 - time1)
            elif 'titarenko' == cfg.stripe_removal_method.lower():
                time1 = time.time()
                print " * Removing stripes/ring artifacts, using the method '{0}'".format(cfg.stripe_removal_method)
                preproc_data = tomopy.prep.stripe.remove_stripe_ti(preproc_data)
                time2 = time.time()
                print " * Removed stripes/ring artifacts, Time elapsed: {0:.3f}".format(time2 - time1)
            else:
                print(" * WARNING: stripe removal method '{0}' is unknown. Not applying it.".
                      format(cfg.stripe_removal_method))
        else:
            print " * Note: not applying stripe removal."

        # Experimental options, disabled and not present in the config objects for now
        # These and related algorithms needs more evaluation/benchmarking
        if False:
            preproc_data = tomopy.misc.corr.adjust_range(preproc_data)

        if False:
            preproc_data = tomopy.prep.normalize.normalize_bg(preproc_data, air=5)

        return preproc_data

    def run_reconstruct_3d(self, proj_data, preproc_cfg, alg_cfg):
        """
        A 3D reconstruction

        @param proj_data :: Input projected images
        @param tool :: reconstruction tool to call/use

        Returns :: reconstructed volume
        """
        num_proj = proj_data.shape[0]
        inc = float(preproc_cfg.max_angle)/(num_proj-1)

        proj_angles=np.arange(0, num_proj*inc, inc)
        # For tomopy
        proj_angles = np.radians(proj_angles)

        verbosity = 1
        if 'astra' == alg_cfg.tool:
            #run_reconstruct_3d_astra(proj_data, algorithm, cor, proj_angles=proj_angles)
            return run_reconstruct_3d_astra_simple(proj_data, proj_angles, alg_cfg, preproc_cfg.cor)

        for slice_idx in [200]: # examples to check: [30, 130, 230, 330, 430]:
            print " > Finding center with tomopy find_center, slice_idx: {0}...".format(slice_idx)
            import tomorec.tool_imports as tti
            try:
                tomopy = tti.import_tomo_tool('tomopy')
                tomopy_cor = tomopy.find_center(tomo=proj_data, theta=proj_angles, ind=slice_idx, emission=False)
                if not preproc_cfg.cor:
                    preproc_cfg.cor = tomopy_cor
                print " > Center of rotation found by tomopy.find_center:  {0}".format(tomopy_cor)
            except ImportError as exc:
                print(" * WARNING: could not import tomopy so could not use the tomopy method to find the center "
                      "of rotation. Details: {0}".format(exc))


        print "Using center of rotation: {0}".format(preproc_cfg.cor)
        start = time.time()
        if 'tomopy' == alg_cfg.tool and 'gridrec' != alg_cfg.algorithm and 'fbp' != alg_cfg.algorithm:
            if not alg_cfg.num_iter:
                alg.cfg_num_iter = tomorec.PreProcConfig.DEF_NUM_ITER
            # For ref, some typical run times with 4 cores:
            # 'bart' with num_iter=20 => 467.640s ~= 7.8m
            # 'sirt' with num_iter=30 => 698.119 ~= 11.63
            if verbosity >= 1:
                print("Running iterative method with tomopy. Algorithm: {0}, "
                      "number of iterations: {1}".format(algorithm, alg_cfg.num_iter))
            rec = tomopy.recon(tomo=proj_data, theta=proj_angles, center=cor,
                               algorithm=alg_cfg.algorithm, num_iter=alg_cfg.num_iter) #, filter_name='parzen')
        else:
            if verbosity >= 1:
                print("Running non-iterative reconstruction algorithm with tomopy. "
                      "Algorithm: {0}".format(alg_cfg.algorithm))
            rec = tomopy.recon(tomo=proj_data, theta=proj_angles, center=preproc_cfg.cor,
                               algorithm=alg_cfg.algorithm)
        tnow = time.time()
        print "Reconstructed 3D volume. Time elapsed in reconstruction algorithm: {0:.3f}".format(tnow - start)

        return rec

    def get_max_frames(self, algorithm):
        frames = 8 if "3D" in algorithm else 1
        return frames

    def astra_reconstruct3d(self, sinogram, angles, depth, alg_cfg):
        # Some of these have issues depending on the GPU setup
        algs_avail = "[FP3D_CUDA], [BP3D_CUDA]], [FDK_CUDA], [SIRT3D_CUDA], [CGLS3D_CUDA]"


        if not alg_cfg.algorithm.upper() in algs_avail:
            raise ValueError("Invalid algorithm requested for the Astra package: {0}. "
                             "Supported algorithms: {1}".format(alg_cfg.algorithm, algs_avail))
        det_rows = sinogram.shape[0]
        det_cols = sinogram.shape[2]

        vol_geom = astra.create_vol_geom(sinograms.shape[0], depth, sinogram.shape[2])
        proj_geom = astra.create_proj_geom('parallel3d', 1.0, 1.0, det_cols,
                                           det_rows, np.deg2rad(angles))

        sinogram_id = astra.data3d.create("-sino", proj_geom, sinogram)
        # Create a data object for the reconstruction
        rec_id = astra.data3d.create('-vol', vol_geom)

        cfg = astra.astra_dict(alg_cfg.algorithm)
        cfg['ReconstructionDataId'] = rec_id
        cfg['ProjectionDataId'] = sinogram_id

        # Create the algorithm object from the configuration structure
        alg_id = astra.algorithm.create(cfg)
        # This will have a runtime in the order of 10 seconds.
        astra.algorithm.run(alg_id, alg_cfg.num_iter)
        #if "CUDA" in params[0] and "FBP" not in params[0]:
        #self.res += astra.algorithm.get_res_norm(alg_id)**2
        #print math.sqrt(self.res)

        # Get the result
        rec = astra.data3d.get(rec_id)

        astra.algorithm.delete(alg_id)
        astra.data3d.delete(rec_id)
        astra.data3d.delete(sinogram_id)

        rec = rec[:160, :160, 1]

        return rec

    def run_reconstruct_3d_astra(self, proj_data, proj_angles, alg_cfg):
        nSinos = get_max_frames(algorithm=algorithm)
        iterations = alg_cfg.num_iter
        print " astra recon - doing {0} iterations".format(iterations)

        # swaps outermost dimensions so it is sinogram layout
        sinogram = proj_data
        sinogram = np.swapaxes(sinogram, 0, 1)


        #ctr = cor
        #width = sinogram.shape[1]
        #pad = 50

        #sino = np.nan_to_num(1./sinogram)

        # pad the array so that the centre of rotation is in the middle
        #alen = ctr
        #blen = width - ctr
        #mid = width / 2.0

        #if ctr > mid:
            #plow = pad
            #phigh = (alen - blen) + pad
        #else:
            #plow = (blen - alen) + pad
            #phigh = pad

        #logdata = np.log(sino+1)

        sinogram = np.tile(sinogram.reshape((1,)+sinogram.shape),
                           (8, 1, 1))

        rec = astra_reconstruct3d(sinogram, proj_angles, depth=nSinos, alg_cfg=alg_cfg)

        return rec

    def run_reconstruct_3d_astra_simple(self, proj_data, proj_angles, alg_cfg, cor=None):
        """
        Run a reconstruction with astra, simple handling of projected data/images

        @param proj_data :: projection images
        @param alg_cfg :: tool/algorithm configuration
        @param cor :: center of rotation
        @param proj_angles :: angles corresponding to the projection images
        """
        sinograms = proj_data

        sinograms = np.swapaxes(sinograms, 0, 1)

        plow = (proj_data.shape[2] - cor*2)
        phigh = 0

        # minval = np.amin(sinograms)
        sinograms = np.pad(sinograms, ((0,0),(0,0),(plow,phigh)), mode='reflect')

        proj_geom = astra.create_proj_geom('parallel3d', .0, 1.0, proj_data.shape[1],
                                           sinograms.shape[2], proj_angles)
        sinogram_id = astra.data3d.create('-sino', proj_geom, sinograms)

        vol_geom = astra.create_vol_geom(proj_data.shape[1], sinograms.shape[2], proj_data.shape[1])
        recon_id = astra.data3d.create('-vol', vol_geom)
        alg_cfg = astra.astra_dict(alg_cfg.algorithm)
        alg_cfg['ReconstructionDataId'] = recon_id
        alg_cfg['ProjectionDataId'] = sinogram_id
        alg_id = astra.algorithm.create(alg_cfg)

        number_of_iters=100
        astra.algorithm.run(alg_id, number_of_iters)
        recon = astra.data3d.get(recon_id)

        astra.algorithm.delete(alg_id)
        astra.data3d.delete(recon_id)
        astra.data3d.delete(sinogram_id)

        return recon

    def apply_postproc_filters(self, recon_data, cfg):
        """
        Apply all post-processing steps/filters/transformations on a reconstructed volume

        @param cfg :: post-processing configuration
        """
        import prep as iprep

        if cfg.circular_mask:
            recon_data = iprep.filters.circular_mask(recon_data, ratio=cfg.circular_mask)
            print " * Applied circular mask on reconstructed volume"
        else:
            print " * Note: not applied circular mask on reconstructed volume"

        if cfg.cut_off_level and cfg.cut_off_level > 0.0:
            print "=== applying cut-off: {0}".format(cfg.cut_off)
            dmin = np.amin(recon_data)
            dmax = np.amax(recon_data)
            rel_cut_off = dmin + cfg.cut_off * (dmax - dmin)
            recon_data[recon_data < rel_cut_off] = dmin

        if cfg.gaussian_filter_par:
            print " * Gaussian filter not implemented"

        if cfg.median_filter_size and cfg.median_filter_size > 1:
            recon_data = scipy.ndimage.median_filter(recon_data, cfg.median_filter_size)
            print (" * Applied median_filter on reconstructed volume, with filtersize: {0}".
                   format(cfg.median_filter_size))
        else:
            print " * Note: not applied median_filter on reconstructed volume"

        if cfg.median_filter3d_size and cfg.median_filter3d_size > 1:
            kernel_size=3
            # Note this can be extremely slow
            recon_data = scipy.signal.medfilt(recon_data, kernel_size=kernel_size)
            print(" * Applied N-dimensional median filter on reconstructed volume, with filter size: {0} ".
                  format(kernel_size))
        else:
            print " * Note: not applied N-dimensional median filter on reconstructed volume"


    def read_in_stack(self, sample_path, img_format):
        """
        Loads a stack, including sample, white and dark images.

        @param sample_path :: path to sample images
        @param img_format :: image format to expect/use (as a filename extension)
        """
        # Note, not giving prefix. It will load all the files found.
        # Example prefixes are prefix = 'tomo_', prefix = 'LARMOR00', prefix = 'angle_agg'

        sample, white, dark = tomoio.read_stack_of_images(sample_path, file_extension=img_format)

        if not isinstance(sample, np.ndarray) or not sample.shape \
           or not isinstance(sample.shape, tuple) or 3 != len(sample.shape):
            raise RuntimeError("Error reading sample images. Could not produce a 3-dimensional array "
                               "of data from the sample images. Got: {0}".format(sample))

        return (sample, white, dark)

    def save_recon_output(self, recon_data, output_dir, save_horiz_slices=False, save_netcdf_vol=False):
        """
        Save output reconstructed volume in different forms.

        @param recon_data :: reconstructed data volume. A sequence of images will be saved from this
        @param output_dir :: where the outputs are being saved
        @param save_horiz_slices :: Save images along the horizontal axis in addition to the vertical
        slices saved by defult. Useful for testing
        @param save_netcdf_vol :: save data into a NetCDF, useful for testing, and easy to load in
        some tools
        """
        # slices along the vertical (z) axis
        # output_dir = 'output_recon_tomopy'
        print "* Saving slices of the reconstructed volume in: {0}".format(output_dir)
        self.save_recon_as_vertical_slices(recon_data, output_dir)

        # Sideways slices:
        save_horiz_slices = False
        if save_horiz_slices:
            out_horiz_dir = os.path.join(output_dir, 'horiz_slices')
            print "* Saving horizontal slices in: {0}".format(out_horiz_dir)
            self.save_recon_as_horizontal_slices(recon_data, out_horiz_dir)

        if save_netcdf_vol:
            print "* Saving reconstructed volume as NetCDF"
            self.save_recon_netcdf(recon_data, output_dir)

    def save_preproc_images(self, output_dir, preproc_data, preproc_cfg,
                            out_dtype='uint16'):
        """
        Save (pre-processed) images from a data array to image files.

        @param output_dir :: where results are being saved, including the pre-proc images/slices
        @param preproc_data :: data volume with pre-processed images
        @param preproc_cfg :: pre-processing configuration set up for a reconstruction
        @param out_dtype :: dtype used for the pixel type/depth in the output image files
        """
        SUBDIR_NAME='preproc_images'

        print " * Pre-processed images (preproc_data) dtype:", preproc_data.dtype
        min_pix = np.amin(preproc_data)
        max_pix = np.amax(preproc_data)
        print "   with min_pix: {0}, max_pix: {1}".format(min_pix, max_pix)
        if preproc_cfg.save_preproc_imgs:
            preproc_dir = os.path.join(output_dir, SUBDIR_NAME)
            print "* Saving pre-processed images into: {0}".format(preproc_dir)
            tomoio.make_dirs_if_needed(preproc_dir)
            for idx in range(0, preproc_data.shape[0]):
                # rescale_intensity has issues with float64=>int16
                tomoio.write_image(preproc_data[idx, :, :], min_pix, max_pix,
                                   os.path.join(preproc_dir, 'out_preproc_proj_image' + str(idx).zfill(6)),
                                   img_format=preproc_cfg.out_img_format, dtype=out_dtype)
        else:
            print "* NOTE: not saving pre-processed images..."

    def save_recon_as_vertical_slices(self, recon_data, output_dir, name_prefix='out_recon_slice',
                                      zero_fill=6):
        """
        Save reconstructed volume (3d) into a series of slices along the Z axis (outermost numpy dimension)

        @param data :: data as images/slices stores in numpy array
        @param output_dir :: where to save the files
        @param name_prefix :: prefix for the names of the images - an index is appended to this prefix
        @param zero_fill :: number of zeros to pad the image/slice index number
        """
        tomoio.make_dirs_if_needed(output_dir)
        #tomopy.io.writer.write_tiff_stack(recon_data)
        min_pix = np.amin(recon_data)
        max_pix = np.amax(recon_data)
        for idx in range(0, recon_data.shape[0]):
            tomoio.write_image(recon_data[idx, :, :], min_pix, max_pix,
                               os.path.join(output_dir, name_prefix + str(idx).zfill(zero_fill)),
                               dtype='uint16')

    def save_recon_as_horizontal_slices(self, recon_data, out_horiz_dir,
                                        name_prefix='out_recon_horiz_slice', zero_fill=6):
        """
        Save reconstructed volume (3d) into a series of slices along the Y axis (second numpy dimension)

        @param data :: data as images/slices stores in numpy array
        @param output_dir :: where to save the files
        @param name_prefix :: prefix for the names of the images throughout the horizontal axis
        @param zero_fill :: number of zeros to pad the image/slice index number. This index is appended to
        the prefix
        """
        tomoio.make_dirs_if_needed(out_horiz_dir)
        for idx in range(0, recon_data.shape[1]):
            tomoio.write_image(recon_data[:, idx, :], np.amin(recon_data), np.amax(recon_data),
                               os.path.join(out_horiz_dir, name_prefix + str(idx).zfill(zero_fill)),
                               dtype='uint16')

    def save_recon_netcdf(self, recon_data, output_dir, filename='tomo_recon_vol.nc'):
        """
        A netCDF, for compatibility/easier interoperation with other tools

        @param recon_data :: reconstructed data volume. A sequence of images will be saved from this
        @param output_dir :: where the outputs are being saved
        @param filename :: name for the NetCDF file
        """
        try:
            from scipy.io import netcdf_file
        except ImportError as exc:
            print " WARNING: could not save NetCDF file. Import error: {0}".format(exc)

        xsize = recon_data.shape[0]
        ysize = recon_data.shape[1]
        zsize = recon_data.shape[2]

        nc_path = os.path.join(output_dir, filename)
        ncfile = netcdf_file(nc_path, 'w')
        ncfile.createDimension('x', xsize)
        ncfile.createDimension('y', ysize)
        ncfile.createDimension('z', zsize)
        print " Creating netCDF volume data variable"
        data = ncfile.createVariable('data', np.dtype('int16').char, ('x','y','z'))
        print " Data shape: {0}".format(data.shape)
        print " Assigning data..."
        data[:, :, :] = recon_data[0:xsize, 0:ysize, 0:zsize]
        print " Closing netCDF file: {0}".format(nc_path)
        ncfile.close()
