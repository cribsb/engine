/*!
 * \file x11/RandR/iRandR_structs.hpp
 * \brief \b Structs: \a internal::_crtc, \a internal::_outpu ,
 * \a internal::_mode , \a internal::_config
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "defines.hpp"

#include <X11/extensions/Xrandr.h>
#include <list>
#include <string>
#include <vector>

namespace e_engine {
namespace internal {

/*!
 * \struct e_engine::internal::_crtc
 * \brief \c [Xlib::XRandR] stores information for a CRTC
 *
 * \note The description is a copy and paste from
 *http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
 *
 * \sa iRandR ; _mode _output
 */
struct _crtc {
   RRCrtc id; //!< The unique \b CRTC id form Xlib

   Time timestamp; //!< [not used]; 'timestamp' indicates when the configuration
                   // was last set.

   /*!
    * 'posX' and 'posY' indicate the position of this CRTC within the screen region.
    * They will be set to 0 when the CRTC is disabled.
    */
   int posX;

   /*!
    * 'posX' and 'posY' indicate the position of this CRTC within the screen region.
    * They will be set to 0 when the CRTC is disabled.
    */
   int posY;

   /*!
    * 'width' and 'height' indicate the size of the area within the screen
    * presented by this CRTC. This may be different than the size of the mode due
    * to rotation, the projective transform, and the Border property described
    * below.  They will be set to 0 when the CRTC is disabled.
    */
   unsigned int width;

   /*!
    * 'width' and 'height' indicate the size of the area within the screen
    * presented by this CRTC. This may be different than the size of the mode due
    * to rotation, the projective transform, and the Border property described
    * below.  They will be set to 0 when the CRTC is disabled.
    */
   unsigned int height;

   /*!
    * 'mode' indicates which mode is active, or None indicating that the CRTC has
    * been disabled and is not displaying the screen contents.
    */
   RRMode mode;

   //! 'rotation' indicates the active rotation. It is set to Rotate_0 when the CRTC is disabled.
   Rotation rotation;

   //! 'rotations' contains the set of rotations and reflections supported by the CRTC
   Rotation rotations;

   /*!
    * 'outputs' is the list of outputs currently connected to this CRTC and is
    * empty when the CRTC is disabled.
    */
   std::vector<RROutput> outputs;

   //! 'possibleOutputs' lists all of the outputs which may be connected to this CRTC
   std::vector<RROutput> possibleOutputs;
};

/*!
 * \struct e_engine::internal::_output
 * \brief \c [Xlib::XRandR] stores information for a output (= display)
 *
 * \note The description is a copy and paste from
 * http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
 *
 * \sa iRandR ; _crtc _mode
 */
struct _output {
   RROutput id; //!< The unique \b CRTC id form Xlib

   Time timestamp; //!< [not used]; 'timestamp' indicates when the configuration was last set.

   /*!
    * 'crtc' is the current source CRTC for video data, or Disabled (= None) if
    * the output is not connected to any CRTC.
    */
   RRCrtc crtc;

   /*!
    * 'name' is a UTF-8 encoded string designed to be presented to the user to
    * indicate which output this is. E.g. "S-Video" or "DVI"
    */
   std::string name;

   /*!
    * 'widthInMillimeters' and 'heightInMillimeters' report the physical
    * size of the displayed area. If unknown, or not really fixed (e.g., for
    * a projector), these values are both zero
    */
   unsigned long mm_width;

   /*!
    * 'widthInMillimeters' and 'heightInMillimeters' report the physical
    * size of the displayed area. If unknown, or not really fixed (e.g., for
    * a projector), these values are both zero
    */
   unsigned long mm_height;

   /*!
    * 'connection' indicates whether the hardware was able to detect a
    * device connected to this output. If the hardware cannot determine
    * whether something is connected, it will set this to UnknownConnection.
    * ==> 0 - Connected; 1 - Disconnected; 2 - Unknown
    */
   Connection connection;

   /*!
    * 'subpixel_order' contains the resulting subpixel order of the
    * connected device to allow correct subpixel rendering.
    */
   SubpixelOrder subpixel_order;

   /*!
    * 'crtcs' is the list of CRTCs that this output may be connected to.
    * Attempting to
    * connect this output to a different CRTC results ina Match error.
    */
   std::vector<RRCrtc> crtcs;

   /*!
    * 'clones' is the list of outputs which may be simultaneously connected to
    * the same CRTC
    * along with this output. Attempting to connect this output with an output
    * not in the 'clones'
    * list results in a Match error.
    */
   std::vector<RROutput> clones;

   /*!
    * 'modes' is the list of modes supported by this output. Attempting to
    * connect this output to
    * a CRTC not using one of these modes results in a Match error.
    */
   std::vector<RRMode> modes;

   /*!
    * The first 'npreferred' modes in 'modes' are preferred by the monitor in
    * some way; for
    * fixed-pixel devices, this would generally indicate which modes match the
    * resolution
    * of the output device.
    */
   int npreferred;
};


/*!
 * \struct e_engine::internal::_mode
 * \brief \c [Xlib::XRandR] stores information for a mode for a CRTC
 *
 * An output mode specifies the complete CRTC timings for
 * a specific mode. The vertical and horizontal synchronization rates
 * can be computed given the dot clock and the h total/v total
 * values. If the dot clock is zero, then all of the timing
 * parameters and flags are not used, and must be zero as this
 * indicates that the timings are unknown or otherwise unused.
 * The name itself will be encoded separately in each usage.
 *
 * \note The description is a copy and paste from
 *http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt
 *
 * \sa iRandR ; _crtc _output
 */
struct _mode {
   RRMode id; //!< The unique \b mode id

   unsigned int  width;      //!< The width of the mode
   unsigned int  height;     //!< The height of the mode
   unsigned long dotClock;   //!< The dotClock (more info: https://de.wikipedia.org/wiki/Pixeltakt)
   unsigned int  hSyncStart; //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int  hSyncEnd;   //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int  hTotal;     //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int  hSkew;      //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int  vSyncStart; //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int  vSyncEnd;   //!< This data is needed for calculating \a refresh and \a syncFreq
   unsigned int  vTotal;     //!< This data is needed for calculating \a refresh and \a syncFreq
   std::string   name;       //!< The name of the mode
   XRRModeFlags  modeFlags;  //!< Some mode flags

   double refresh;  //!< The refresh rate
   double syncFreq; //!< The sync frequnece

   /*!
    * \brief Only used for the STL list method unique();
    *
    * \returns Whether the objects are equal or not.
    */
   inline bool operator==( const _mode &_m ) const {
      if ( _m.id == id && _m.height == height && _m.width == width && _m.refresh == refresh &&
           _m.syncFreq == syncFreq && _m.name == name )
         return true;

      return false;
   }

   /*!
    * \brief Only used for the STL list method sort();
    *
    * \returns Whether the object is greater than the other.
    */
   inline bool operator<( const _mode &_m ) const {
      if ( width > _m.width )
         return true;
      else if ( width < _m.width )
         return false;

      if ( height > _m.height )
         return true;
      else if ( height < _m.height )
         return false;

      if ( refresh < _m.refresh )
         return true;
      else if ( refresh > _m.refresh )
         return false;

      return true;
   }
};

/*!
 * \struct e_engine::internal::_config
 * \brief Stores all necessary infomation to restore a specific RandR
 * configuration
 */
struct _config {
   RROutput primary; //!< The primary output

   std::vector<XRRCrtcGamma *> gamma;    //!< Holds gamma information for each CRTC
   std::vector<_crtc>          CRTCInfo; //!< All important data to restore every CRTC
};
}
}
