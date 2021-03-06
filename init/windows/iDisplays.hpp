/*!
 * \file windows/iDisplays.hpp
 * \brief \b Classes: \a iDisplays
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

#include "iDisplayBasic.hpp"
#include <windows.h>

namespace e_engine {

namespace windows_win32 {

class iRandR_win32;

/*!
 * \brief Contains information about the current Display
 *
 * Changes in objects of this class wont impact the current display
 * configuration. See iRandR for more details
 *
 * The only way to get a object of this class is calling iRandR::getDisplay()
 *
 * \note To apply the changes made here, you must call iRandR::setDisplaySizes()
 *       first and then iRandR::applyNewSettings()
 */
class INIT_API iDisplays : public iDisplayBasic {
 private:
   //! \brief internal structure for storing important mode information.
   std::vector<DEVMODEW> vModes_V_win32;                 //!< all possible modes
   DEVMODEW              vCurrentSettings_win32;         //!< The current display settings
   DEVMODEW              vSelectedDisplaySettings_win32; //!< The selected display settings

   //! The Winapi display device (stores information about the display)
   DISPLAY_DEVICEW vDisplayDevice_win32;

   iDisplays() {}
   iDisplays( std::wstring _name, bool _enabled, bool _isPrimary );

   //! \brief Add a new DEVMODEW to the internal mode list
   void addMode( DEVMODEW _mode ) { vModes_V_win32.push_back( _mode ); }

   double findNearestFreqTo( double       _rate,
                             unsigned int _width,
                             unsigned int _height,
                             DEVMODEW &   _mode,
                             double &     _diff ) const;

   //! \brief Sets the current settings (DEVMODEW)
   void setCurrentSettings( DEVMODEW _current ) {
      vSelectedDisplaySettings_win32 = vCurrentSettings_win32 = _current;
   }
   //! \brief Set the display device (needed for the disply ID)
   void setDisplayDevice( DISPLAY_DEVICEW _device ) { vDisplayDevice_win32 = _device; }
   DEVMODEW                               getSelectedDevmode() const;
   //! \brief Get the display device (needed for the disply ID)
   DISPLAY_DEVICEW getDisplayDevice() const { return vDisplayDevice_win32; }

   int getMaxBitsPerPelFromResolutionAndFreq( unsigned int _width,
                                              unsigned int _height,
                                              double       _rate ) const;

 public:
   virtual ~iDisplays() {}

   void autoSelectBest();

   void disable();
   void enable();

   std::vector<iDisplayBasic::res> getPossibleResolutions() const;
   bool isSizeSupported( unsigned int _width, unsigned int _height ) const;

   std::vector<double> getPossibleRates( unsigned int _width, unsigned int _height ) const;

   double autoSelectBySize( unsigned int _width,
                            unsigned int _height,
                            double       _preferedRate = 0,
                            double       _maxDiff      = 1 );
   bool select( unsigned int _width, unsigned int _height, double _rate );

   void setNoClones() {}
   void setCloneOf( iDisplays const & ) {}

   void getSelectedRes( unsigned int &_width, unsigned int &_height, double &_rate ) const;

   friend class iRandR;
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################

/*!
 * \fn void iDisplays::setNoClones()
 * \brief Does nothing because Cloning not supported with windows
 *
 * \returns Nothing
 */

/*!
 * \fn void iDisplays::setCloneOf()
 * \brief Does nothing because Cloning not supported with windows
 *
 * \returns Nothing
 */

} // windows_win32

} // e_engine


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
