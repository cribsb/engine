/*!
 * \file windows/iWindow.hpp
 * \brief \b Classes: \a iWindow
 *
 * This file contains the class \b iWindow which creates
 * the window in Windows and the OpenGL context on it.
 *
 * \sa e_context.cpp e_iInit.cpp
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

#include "iEventInfo.hpp"
#include "iKeyboard.hpp"
#include "iRandR.hpp"
#include "iWindowBasic.hpp"

namespace e_engine {

namespace windows_win32 {


class INIT_API iWindow : public iKeyboard, public iRandR, public iWindowBasic {
 private:
   HINSTANCE vInstance_win32;
   WNDCLASSW vWindowClass_win32;
   HWND vHWND_Window_win32;
   RECT vWindowRect_win32;
   LPCWSTR vClassName_win32;

   static LRESULT CALLBACK initialWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
   static LRESULT CALLBACK staticWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );
   LRESULT CALLBACK actualWndProc( UINT _uMsg,
                                   WPARAM _wParam,
                                   LPARAM _lParam,
                                   iEventInfo _tempInfo );

   bool vWindowsCallbacksError;

   bool vHasWindow;


   bool vIsMouseGrabbed;
   bool vIsCursorHidden;

   bool setWindowState( UINT _flags, HWND _pos = (HWND)1000 );

   virtual void makeEInitEventBasicAbstract() {}

 protected:
   bool vWindowsDestroy;
   bool vWindowsNCDestroy;

   HWND getHWND_win32() { return vHWND_Window_win32; }

 public:
   iWindow();
   virtual ~iWindow() {
      if ( vHasWindow )
         destroyWindow();
   }

   int createWindow() override;

   void fullScreen( ACTION _action, bool _allMonitors = false ) override;
   void destroyWindow() override;

   void setAttribute( ACTION _action,
                      WINDOW_ATTRIBUTE _type1,
                      WINDOW_ATTRIBUTE _type2 = NONE ) override;


   int setFullScreenMonitor( iDisplays & ) { return 0; }
   void setDecoration( ACTION _action ) override;
   void changeWindowConfig( unsigned int _width,
                            unsigned int _height,
                            int _posX,
                            int _posY ) override;
   bool fullScreenMultiMonitor() { return false; }

   bool grabMouse() override;
   void freeMouse() override;
   bool getIsMouseGrabbed() const override;

   void moveMouse( unsigned int _posX, unsigned int _posY ) override;

   void hideMouseCursor() override;
   void showMouseCursor() override;
   bool getIsCursorHidden() const override;

   //       unsigned getVertexArrayOpenGL() { return vVertexArray_OGL; }
};

/*!
 * \fn iWindow::setFullScreenMonitor
 * \brief Not supported with Windows
 *
 * \note Does Nothing
 *
 * \todo Support more than one fullscreen monitor in Windows
 *
 * \returns 0
 */

/*!
 * \fn iWindow::fullScreenMultiMonitor
 * \brief Not supported with Windows
 *
 * \note Does Nothing
 *
 * \todo Support more than one fullscreen monitor in Windows
 *
 * \returns false
 */

namespace internal {

/*!
 * \brief Stores information about WIN32 window classes
 *
 * Window classes are registered globaly for one application.
 * Registering them more than once leads to errors. This class
 * stores if a window class is registered.
 */
class eWindowClassRegister {
 private:
   bool vClass1Registered; //!< Is the temporary window class registered (used for the temporary
   // OpenGL context)
   bool vClass2Registered; //!< Is the final window class registered (the "real" window class)
 public:
   //! Both window classes are not registered when the application starts
   eWindowClassRegister() : vClass1Registered( false ), vClass2Registered( false ) {}

   bool getC1() { return vClass1Registered; } //!< Is the temporary window class already registered?
   bool getC2() { return vClass2Registered; } //!< Is the final window class already registered?

 private:
   void setC1() {
      vClass1Registered = true;
   } //!< Set the temporary window class as registered (only windows_win32::iWindow can do this)
   void setC2() {
      vClass2Registered = true;
   } //!< Set the final window class as registered (only windows_win32::iWindow can do this)

   friend class e_engine::windows_win32::iWindow;
};

//! Global object that stores the state of the window classes
extern eWindowClassRegister CLASS_REGISTER;

} // internal

} // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
