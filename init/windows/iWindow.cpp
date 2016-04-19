/*!
 * \file windows/iWindow.cpp
 * \brief \b Classes: \a iWindow
 *
 * This file contains the class \b iWindow which creates
 * the window in Windows and the OpenGL context on it.
 *
 * Please note that the actualWndProc, the method for resolving
 * the window events is located within event.cpp
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

#define VK_USE_PLATFORM_WIN32_KHR ;

#include <windows.h>
#include "iWindow.hpp"
#include "iInit.hpp"
#include "uLog.hpp"
#include "uEnum2Str.hpp"

namespace {

template <class T>
inline std::string numToSizeStringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR = std::to_string( _val );
   if ( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}
}

namespace e_engine {

namespace windows_win32 {

namespace internal {
eWindowClassRegister CLASS_REGISTER;
}

LRESULT CALLBACK __WndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam );

iWindow::iWindow( iInit *_init ) {
   vWindowsCallbacksError = false;
   vHasWindow             = false;

   vIsCursorHidden = false;
   vIsMouseGrabbed = false;

   vPointers         = std::make_unique<ClassPointers>();
   vPointers->window = this;
   vPointers->init   = _init;
}

/*!
* \brief Creates a Windows window
*
* \returns -1 when RegisterClass failed
* \returns 0  on success
* \returns 1  if there is already a window
* \returns 2  if there was a windows callback error
*/
int iWindow::createWindow() {
   if ( vHasWindow )
      return 1;


   vHWND_Window_win32 = 0;
   vInstance_win32    = 0;

   vClassName_win32             = L"VK_CLASS";
   LPCSTR lClassName_TEMP_win32 = "VK_CLASS_TEMP";

   DWORD lWinStyle;
   DWORD lExtStyle;

   if ( GlobConf.win.windowDecoration && !GlobConf.win.fullscreen ) {
      lWinStyle = WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_CAPTION | WS_SYSMENU |
                  WS_MINIMIZEBOX;
      lExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
   } else {
      lWinStyle = WS_POPUP;
      lExtStyle = WS_EX_APPWINDOW;
   }


   //
   // Create the actual context
   //


   vInstance_win32 = GetModuleHandle( NULL );

   if ( !internal::CLASS_REGISTER.getC2() ) {
      vWindowClass_win32.style =
            CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // We want a unique DC and redraw on window changes
      vWindowClass_win32.lpfnWndProc   = &iInit::initialWndProc;
      vWindowClass_win32.cbClsExtra    = 0; // We do not need this
      vWindowClass_win32.cbWndExtra    = sizeof( ClassPointers );
      vWindowClass_win32.hInstance     = vInstance_win32;
      vWindowClass_win32.hIcon         = NULL; // We dont have a special icon
      vWindowClass_win32.hCursor       = LoadCursor( NULL, IDC_ARROW ); // Take the default mouse cursor
      vWindowClass_win32.hbrBackground = NULL;                          // We dont need a background
      vWindowClass_win32.lpszMenuName  = NULL;                          // We dont want a menu
      vWindowClass_win32.lpszClassName = vClassName_win32;


      if ( RegisterClassW( &vWindowClass_win32 ) == 0 ) {
         eLOG( "Failed to register the (final) window class ", (uint64_t)GetLastError() );
         return -1;
      }


      internal::CLASS_REGISTER.setC2();
   }

   if ( vWindowsCallbacksError ) {
      eLOG( "Problems with window callback" );
      return 2;
   }

   if ( GlobConf.win.fullscreen ) {
      HWND lDesktopHWND_win32 = GetDesktopWindow();

      if ( GetWindowRect( lDesktopHWND_win32, &vWindowRect_win32 ) == 0 ) {
         vWindowRect_win32.left   = GlobConf.win.posX;
         vWindowRect_win32.right  = GlobConf.win.posX + GlobConf.win.width;
         vWindowRect_win32.top    = GlobConf.win.posY;
         vWindowRect_win32.bottom = GlobConf.win.posY + GlobConf.win.height;
         wLOG( "Fullscreen failed" );
      }

      ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
   } else {
      vWindowRect_win32.left   = GlobConf.win.posX;
      vWindowRect_win32.right  = GlobConf.win.posX + GlobConf.win.width;
      vWindowRect_win32.top    = GlobConf.win.posY;
      vWindowRect_win32.bottom = GlobConf.win.posY + GlobConf.win.height;
   }

   GlobConf.win.posX   = vWindowRect_win32.left;
   GlobConf.win.posY   = vWindowRect_win32.top;
   GlobConf.win.width  = vWindowRect_win32.right - vWindowRect_win32.left;
   GlobConf.win.height = vWindowRect_win32.bottom - vWindowRect_win32.top;

   // Now do the same again, but this time create the actual window
   AdjustWindowRectEx( &vWindowRect_win32, lWinStyle, false, lExtStyle );
   std::wstring lWindowName_wstr( GlobConf.config.appName.begin(), GlobConf.config.appName.end() );
   iLOG( "Window Name: ", lWindowName_wstr );
   vHWND_Window_win32 = CreateWindowExW( // The W  is required for it to be a Unicode window
         lExtStyle,                      // Extended window style
         vClassName_win32,               // Window class name
         lWindowName_wstr.c_str(),       // Window Name (converted to a wide string)
         lWinStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // Window style
         GlobConf.win.posX,                             // X
         GlobConf.win.posY,                             // Y
         GlobConf.win.width,                            // Width
         GlobConf.win.height,                           // Height
         NULL,                                          // No parent window
         NULL,                                          // No menu
         vInstance_win32,                               // The instance
         vPointers.get()                                // We dont want special window creation
         );

   /*!
   *\todo: Changed the vClassName_win32 and Windowname into a LPCWSTR,
   * Changed the vWindowClass_win32 into a WNDCLASSW,
   * used CreateWindowExW and RegisterClassW( &vWindowClass_win32 )
   * See http://technet.microsoft.com/en-ca/dd319108%28v=vs.90%29.aspx
   */


   ShowCursor( TRUE );

   ShowWindow( vHWND_Window_win32, SW_SHOW );
   SetForegroundWindow( vHWND_Window_win32 );
   SetFocus( vHWND_Window_win32 );

   vHasWindow = true;

   vWindowsDestroy   = false;
   vWindowsNCDestroy = false;

   return 1;
}

void iWindow::destroyWindow() {
   if ( !vHasWindow )
      return;

   iLOG( "Destroying everything" );


   /*
    * DestroyWindow( vHWND_Window_win32 );
    *
    * Won't work here because:
    *
    *  1. Windows
    *  2. Must be called in the thread where the window was created
    *
    * Everything else is done in the event loop.
    */

   vHasWindow = false;
}

/*!
 * \brief Changes the window config
 * \param _width  The new width
 * \param _height The new height
 * \param _posX   The new X coordinate
 * \param _posY   The new Y coordinate
 * \returns The return value of \c SetWindowPos
 */
void iWindow::changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY ) {
   GlobConf.win.width  = _width;
   GlobConf.win.height = _height;
   GlobConf.win.posX   = _posX;
   GlobConf.win.posY   = _posY;

   SetWindowPos( vHWND_Window_win32, HWND_TOP, _posX, _posY, _width, _height, SWP_SHOWWINDOW );
}


/*!
 * \brief Sets the window state
 *
 * Uses SetWindowPos to set some states
 *
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms633545%28v=vs.85%29.aspx
 *
 * \param[in] _flags the uFlags
 * \param[in] _pos   the hWndInsertAfter option (default: keep)
 *
 * \returns the return value of SetWindowPos
 */
bool iWindow::setWindowState( UINT _flags, HWND _pos ) {
   if ( _pos == (HWND)1000 )
      _flags |= SWP_NOZORDER;

   return SetWindowPos( vHWND_Window_win32, _pos, 0, 0, 10, 10, _flags | SWP_NOSIZE | SWP_NOMOVE );
}


void iWindow::setWindowType( WINDOW_TYPE _type ) {}

void iWindow::setWindowNames( std::string _windowName, std::string _iconName ) {}

/*!
 * \brief Changes the state of the window
 *
 * \param[in] _action What to do
 * \param[in] _type1  The first attribute to change
 * \param[in] _type2  The second attribute to change (Default: NONE)
 *
 * \warning Only C_ADD supported; C_REMOVE and C_TOGGLE are treated as C_ADD
 *
 * \sa e_engine::ACTION, e_engine::WINDOW_ATTRIBUTE
 */
void iWindow::setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 ) {
   if ( _type1 == _type2 ) {
      eLOG( "Changing the same attribute at the same time makes no sense. Aborting." );
      return;
   }

   std::string lMode_STR;
   std::string lState1_str = "NOTHING", lState2_str = "NOTHING";
   bool lState1Supported = false, lState2Supported = false;

   // clang-format off
   switch ( _type1 ) {
      case MODAL:        lState1_str        = "MODAL";        break;
      case STICKY:       lState1_str        = "STICKY";       break;
      case SHADED:       lState1_str        = "SHADED";       break;
      case SKIP_TASKBAR: lState1_str        = "SKIP_TASKBAR"; break;
      case SKIP_PAGER:   lState1_str        = "SKIP_PAGER";   break;
      default:           lState1Supported = true;           break;
   }

   switch ( _type2 ) {
      case MODAL:        lState2_str        = "MODAL";        break;
      case STICKY:       lState2_str        = "STICKY";       break;
      case SHADED:       lState2_str        = "SHADED";       break;
      case SKIP_TASKBAR: lState2_str        = "SKIP_TASKBAR"; break;
      case SKIP_PAGER:   lState2_str        = "SKIP_PAGER";   break;
      default:           lState2Supported = true;           break;
   }
   // clang-format on

   if ( !lState1Supported ) {
      wLOG( "Window attribute ", lState1_str, " is not supported on Windows. Reverting to NONE" );
      _type1      = NONE;
      lState1_str = "NOT_SUPPORTED";
   }

   if ( !lState2Supported ) {
      wLOG( "Window attribute ", lState1_str, " is not supported on Windows. Reverting to NONE" );
      _type2      = NONE;
      lState2_str = "NOT_SUPPORTED";
   }

   if ( _type1 == NONE && _type2 == NONE ) {
      eLOG( "No supported window attribute found. Aborting." );
      return;
   }

   switch ( _action ) {
      case C_REMOVE: lMode_STR = "Removed"; break;
      case C_ADD: lMode_STR    = "Enabled"; break;
      case C_TOGGLE: lMode_STR = "Toggled"; break;
      default: return;
   }

   HWND lDesktopHWND_win32 = GetDesktopWindow();
   RECT lDesktopRect_win32;

   if ( _type1 != NONE ) {
      switch ( _type1 ) {
         case HIDDEN:
            setWindowState( SWP_HIDEWINDOW );
            lState1_str = "HIDDEN";
            break;
         case FULLSCREEN:
            fullScreen( _action );
            lState1_str = "FULLSCREEN";
            break;
         case ABOVE:
            setWindowState( 0, HWND_TOP );
            lState1_str = "ABOVE";
            break;
         case BELOW:
            setWindowState( 0, HWND_BOTTOM );
            lState1_str = "BELOW";
            break;
         case DEMANDS_ATTENTION: lState1_str = "DEMANDS_ATTENTION";
         case FOCUSED:
            ShowWindow( vHWND_Window_win32, SW_SHOW );
            SetForegroundWindow( vHWND_Window_win32 );
            SetFocus( vHWND_Window_win32 );
            if ( lState1_str != "DEMANDS_ATTENTION" )
               lState1_str = "FOCUSED";
            break;
         case MAXIMIZED_VERT:
            GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );
            changeWindowConfig( GlobConf.win.width,
                                lDesktopRect_win32.bottom - lDesktopRect_win32.top,
                                GlobConf.win.posX,
                                lDesktopRect_win32.top );
            lState1_str = "MAXIMIZED_VERT";
            break;
         case MAXIMIZED_HORZ:
            GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );
            changeWindowConfig( lDesktopRect_win32.right - lDesktopRect_win32.left,
                                GlobConf.win.height,
                                lDesktopRect_win32.left,
                                GlobConf.win.posY );
            lState1_str = "MAXIMIZED_HORZ";
            break;
         default: return;
      }
   }




   if ( _type2 != NONE ) {
      switch ( _type2 ) {
         case HIDDEN:
            setWindowState( SWP_HIDEWINDOW );
            lState2_str = "HIDDEN";
            break;
         case FULLSCREEN:
            fullScreen( _action );
            lState2_str = "FULLSCREEN";
            break;
         case ABOVE:
            setWindowState( 0, HWND_TOP );
            lState2_str = "ABOVE";
            break;
         case BELOW:
            setWindowState( 0, HWND_BOTTOM );
            lState2_str = "BELOW";
            break;
         case DEMANDS_ATTENTION: lState2_str = "DEMANDS_ATTENTION";
         case FOCUSED:
            ShowWindow( vHWND_Window_win32, SW_SHOW );
            SetForegroundWindow( vHWND_Window_win32 );
            SetFocus( vHWND_Window_win32 );
            if ( lState2_str != "DEMANDS_ATTENTION" )
               lState2_str = "FOCUSED";
            break;
         case MAXIMIZED_VERT:
            GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );
            changeWindowConfig( GlobConf.win.width,
                                lDesktopRect_win32.bottom - lDesktopRect_win32.top,
                                GlobConf.win.posX,
                                lDesktopRect_win32.top );
            lState2_str = "MAXIMIZED_VERT";
            break;
         case MAXIMIZED_HORZ:
            GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );
            changeWindowConfig( lDesktopRect_win32.right - lDesktopRect_win32.left,
                                GlobConf.win.height,
                                lDesktopRect_win32.left,
                                GlobConf.win.posY );
            lState2_str = "MAXIMIZED_HORZ";
            break;
         default: return;
      }
   }

   iLOG( lMode_STR, " window attribute ", lState1_str, " and ", lState2_str );
}


void iWindow::setDecoration( ACTION _action ) {
   bool lGlobConfOld = GlobConf.win.windowDecoration;

   switch ( _action ) {
      case C_ADD: GlobConf.win.windowDecoration    = true; break;
      case C_REMOVE: GlobConf.win.windowDecoration = false; break;
      case C_TOGGLE: GlobConf.win.windowDecoration = !GlobConf.win.windowDecoration; break;

      default:
         eLOG( "This message is theoretically impossible! [bool iWindow::setDecoration( "
               "ACTION _action )]" );
         return;
   }

   if ( lGlobConfOld != GlobConf.win.windowDecoration ) {
      iLOG( "Window decoration ( ",
            GlobConf.win.windowDecoration ? "enabled" : "disabled",
            " ) needs window restart!" );
   }
}

/*!
 * \brief Sets or removes fullscreen
 * \note You need to restart to apply the changes
 * \param[in] _action What to do
 *
 * \TODO Implement borderless
 */
void iWindow::fullScreen( ACTION _action, bool ) {
   bool lGlobConfOld = GlobConf.win.fullscreen;

   switch ( _action ) {
      case C_ADD: GlobConf.win.fullscreen    = true; break;
      case C_REMOVE: GlobConf.win.fullscreen = false; break;
      case C_TOGGLE: GlobConf.win.fullscreen = !GlobConf.win.fullscreen; break;

      default:
         eLOG( "This message is theoretically impossible! [bool iWindow::setDecoration( "
               "ACTION _action )]" );
         return;
   }

   if ( lGlobConfOld != GlobConf.win.fullscreen ) {
      iLOG( "Fullscreen ( ",
            GlobConf.win.fullscreen ? "enabled" : "disabled",
            " ) needs window restart!" );
   }
}


/*!
 * \brief Grabs the mouse pointer (and the keyboard)
 *
 * \note You can only grab the mouse if it is not already grabbed by this window
 *
 * \returns true if successful and false if not
 */
bool iWindow::grabMouse() {
   if ( vIsMouseGrabbed ) {
      wLOG( "Mouse is already grabbed" );
      return false;
   }

   RECT bounds; // The Rectangle that the Cursor will be forced to be in

   // GetWindowRect(vHWND_Window_win32, &bounds);

   // The following allows for more customization
   bounds.left   = GlobConf.win.posX;
   bounds.top    = GlobConf.win.posY;
   bounds.right  = GlobConf.win.posX + GlobConf.win.width;
   bounds.bottom = GlobConf.win.posY + GlobConf.win.height;

   if ( ClipCursor( &bounds ) == 0 ) {
      wLOG( "Error while grabbing mouse: ", (uint64_t)GetLastError() );
      return false;
   }
   vIsMouseGrabbed = true;
   iLOG( "Mouse grabbed" );
   return true;
}

void iWindow::freeMouse() {
   if ( !vIsMouseGrabbed ) {
      wLOG( "Mouse is not grabbed" );
      return;
   }
   if ( ClipCursor( NULL ) == 0 ) { // Reset the bounds
      wLOG( "Error while freeing mouse: ", (uint64_t)GetLastError() );
      return;
   }
   vIsMouseGrabbed = false;
   iLOG( "Mouse ungrabbed" );
}


/*!
 * \brief Get if the mouse is grabbed
 * \returns if the mouse is grabbed
 */
bool iWindow::getIsMouseGrabbed() const { return vIsMouseGrabbed; }


/*!
 * \brief Sets the mouse position
 *
 * \param[in] _posX The x coordinate in our window
 * \param[in] _posY The y coordinate in our window
 *
 * \note _posX and _posY must be inside our window
 *
 * \returns true if successful and false if not
 */
void iWindow::moveMouse( unsigned int _posX, unsigned int _posY ) {
   if ( _posX > GlobConf.win.width || _posY > GlobConf.win.height ) {
      wLOG( "_posX and/or _posY outside the window" );
      return;
   }

   int result = SetCursorPos( GlobConf.win.posX + _posX, GlobConf.win.posY + _posY );

   if ( result == 0 ) {
      wLOG( "Error while setting mouse position: ", (uint64_t)GetLastError() );
   }
}


/*!
 * \brief Hides the cursor
 * \returns true if successful and false if not
 */
void iWindow::hideMouseCursor() {
   if ( vIsCursorHidden ) {
      wLOG( "Cursor is already hidden" );
      return;
   }

   int showValue = ShowCursor( false );
   while ( showValue > -1 ) {
      showValue = ShowCursor( false );
   }

   vIsCursorHidden = true;
   iLOG( "Cursor hidden" );
}

/*!
 * \brief Shows the cursor
 * \returns true if successful and false if not
 */
void iWindow::showMouseCursor() {
   if ( !vIsCursorHidden ) {
      wLOG( "Cursor is already visible" );
      return;
   }

   int showValue = ShowCursor( true );
   while ( showValue < 0 ) {
      showValue = ShowCursor( true );
   }

   vIsCursorHidden = false;
   iLOG( "Cursor visible" );
}

/*!
 * \brief Get if the cursor is hidden
 * \returns true if the cursor is hidden
 */
bool iWindow::getIsCursorHidden() const { return vIsCursorHidden; }

bool iWindow::getIsWindowCreated() const { return vHasWindow; }

VkSurfaceKHR iWindow::getVulkanSurface( VkInstance _instance ) {
   if ( !vHasWindow ) {
      eLOG( "Cannot create vulkan surface when there is no window." );
      return nullptr;
   }
   VkSurfaceKHR lSurface = nullptr;
   VkWin32SurfaceCreateInfoKHR lCreateInfo;

   lCreateInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
   lCreateInfo.pNext     = nullptr;
   lCreateInfo.flags     = 0;
   lCreateInfo.hinstance = vInstance_win32;
   lCreateInfo.hwnd      = vHWND_Window_win32;

   VkResult lResult = vkCreateWin32SurfaceKHR( _instance, &lCreateInfo, nullptr, &lSurface );

   if ( lResult != VK_SUCCESS ) {
      eLOG( "An error occurred in 'vkCreateWin32SurfaceKHR': ", uEnum2Str::toStr( lResult ) );
   }

   return lSurface;
}

// Temp wndProc
LRESULT CALLBACK __WndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   switch ( _uMsg ) {
      default: break;
   }
   return DefWindowProc( _hwnd, _uMsg, _wParam, _lParam );
}


} // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
