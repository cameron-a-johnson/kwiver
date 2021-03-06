/*ckwg +29
 * Copyright 2012-2018 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 /**
 * \file
 * \brief Source file for world_space
 */


#include "world_space.h"

#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vgl/vgl_box_2d.h>


namespace kwiver {
namespace arrows {
namespace super3d {

world_space::world_space(unsigned int pixel_width, unsigned int pixel_height)
  : ni_(pixel_width), nj_(pixel_height)
{
  wip.set_fill_unmapped(true);
  wip.set_interpolator(kwiver::arrows::super3d::warp_image_parameters::LINEAR);
}

//*****************************************************************************

std::vector<vpgl_perspective_camera<double> >
world_space
::warp_cams(const std::vector<vpgl_perspective_camera<double> > &cameras,
            int ref_frame) const
{
  return cameras;
}

//*****************************************************************************

/// warps image \in to the world volume at depth_slice,
/// uses ni and nj as out's dimensions
template<typename PixT>
void world_space::warp_image_to_depth(const vil_image_view<PixT> &in,
                                      vil_image_view<PixT> &out,
                                      const vpgl_perspective_camera<double> &cam,
                                      double depth_slice, int f, PixT fill)
{
  wip.set_unmapped_value(fill);
  std::vector<vnl_double_3> wpts = this->get_slice(depth_slice);

  std::vector<vgl_homg_point_2d<double> > warp_pts;
  std::vector<vgl_homg_point_2d<double> > proj_pts;

  warp_pts.push_back(vgl_homg_point_2d<double>(0.0, 0.0, 1.0));
  warp_pts.push_back(vgl_homg_point_2d<double>(ni_, 0.0, 1.0));
  warp_pts.push_back(vgl_homg_point_2d<double>(ni_, nj_, 1.0));
  warp_pts.push_back(vgl_homg_point_2d<double>(0.0, nj_, 1.0));

  for (unsigned int i = 0; i < wpts.size(); i++)
  {
    double u, v;
    cam.project(wpts[i][0], wpts[i][1], wpts[i][2], u, v);
    proj_pts.push_back(vgl_homg_point_2d<double>(u, v, 1));
  }

  vgl_h_matrix_2d<double> H;
  vgl_h_matrix_2d_compute_4point dlt;
  dlt.compute(warp_pts, proj_pts, H);

  out.set_size(ni_, nj_, 1);
  warp_image(in, out, vgl_h_matrix_2d<double>(H), wip);

#if 0
  //image writing for debugging
  vil_image_view<double> outwrite;
  outwrite.deep_copy(out);
  vil_math_scale_and_offset_values(outwrite, 255.0, 0.0);
  vil_image_view<vxl_byte> to_save;
  vil_convert_cast<double, vxl_byte>(outwrite, to_save);
  char buf[60];
  sprintf(buf, "images/slice%2f_frame%d_%d.png", wpts[0][2], f, wip.interpolator_);
  vil_save(to_save, buf);
#endif
}

//*****************************************************************************

/// Return a subset of landmark points that project into the given region of interest
std::vector<vnl_double_3>
filter_visible_landmarks(const vpgl_perspective_camera<double> &camera,
  int i0, int ni, int j0, int nj,
  const std::vector<vnl_double_3> &landmarks)
{
  vgl_box_2d<double> box(i0, i0 + ni, j0, j0 + nj);
  std::vector<vnl_double_3> visible_landmarks;
  std::cout << "filtering landmarks using ROI " << box << "\n";
  for (unsigned int i = 0; i < landmarks.size(); i++)
  {
    const vnl_double_3 &p = landmarks[i];
    double u, v;
    camera.project(p[0], p[1], p[2], u, v);
    if (box.contains(u, v))
    {
      visible_landmarks.push_back(p);
    }
  }
  std::cout << "ratio of filtered landmarks in ROI: "
    << visible_landmarks.size() << "/" << landmarks.size() << std::endl;
  return visible_landmarks;
}

//*****************************************************************************

/// Robustly compute the bounding planes of the landmarks in a given direction
void
compute_offset_range(const std::vector<vnl_double_3> &landmarks,
  const vnl_vector_fixed<double, 3> &normal,
  double &min_offset, double &max_offset,
  const double outlier_thresh,
  const double safety_margin_factor)
{
  min_offset = std::numeric_limits<double>::infinity();
  max_offset = -std::numeric_limits<double>::infinity();

  std::vector<double> offsets;

  for (unsigned int i = 0; i < landmarks.size(); i++)
  {
    offsets.push_back(dot_product(normal, landmarks[i]));
  }
  std::sort(offsets.begin(), offsets.end());

  const unsigned int min_index =
    static_cast<unsigned int>((offsets.size() - 1) * outlier_thresh);
  const unsigned int max_index = offsets.size() - 1 - min_index;
  min_offset = offsets[min_index];
  max_offset = offsets[max_index];

  const double safety_margin = safety_margin_factor * (max_offset - min_offset);
  max_offset += safety_margin;
  min_offset -= safety_margin;
}

//*****************************************************************************


template void world_space::warp_image_to_depth(const vil_image_view<double> &in,
                                               vil_image_view<double> &out,
                                               const vpgl_perspective_camera<double> &cam,
                                               double depth_slice, int f, double fill);

template void world_space::warp_image_to_depth(const vil_image_view<vxl_byte> &in,
                                               vil_image_view<vxl_byte> &out,
                                               const vpgl_perspective_camera<double> &cam,
                                               double depth_slice, int f, vxl_byte fill);

template void world_space::warp_image_to_depth(const vil_image_view<bool> &in,
                                               vil_image_view<bool> &out,
                                               const vpgl_perspective_camera<double> &cam,
                                               double depth_slice, int f, bool fill);

} // end namespace super3d
} // end namespace arrows
} // end namespace kwiver


