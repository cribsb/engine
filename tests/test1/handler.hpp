/*!
 * \file handler.hpp
 * \brief Class MyHandler
 */

#include <engine.hpp>

#ifndef HANDLER_HPP
#define HANDLER_HPP

using namespace std;
using namespace e_engine;
using namespace OS_NAMESPACE;

class MyHandler : public rWorld {
      typedef uSlot<void, MyHandler, iEventInfo> _SLOT_;
   private:
      vector<iDisplays> vDisp_RandR;

      string  vDataRoot_str;

      rNormalObject vObject1;

      GLfloat vAlpha;
      
      GLfloat vCurrentRot;
      
      rVec3f  vCameraPos;
      rVec3f  vCameraLook;
      rVec3f  vCameraUp;
      rVec3f  vCameraLookWorker;
      
      iInit  *vInitPointer;

      MyHandler() : rWorld( nullptr ), vObject1( "OBJ_1" ) {}
   public:
      _SLOT_ slotWindowClose;
      _SLOT_ slotResize;
      _SLOT_ slotKey;
      _SLOT_ slotMouse;
      MyHandler( string _dataRoot, iInit *_init ) : 
      rWorld( _init ), 
      vObject1( "OBJ_1" ),
      vCameraPos( 0, 0, 0 ),
      vCameraLook( 0, 0, 1 ),
      vCameraUp( 0, 1, 0 ),
      vCameraLookWorker( 0, 0, 0 ),
      vInitPointer( _init )
      {
         slotWindowClose.setFunc( &MyHandler::windowClose, this );
         slotResize.setFunc( &MyHandler::resize, this );
         slotKey.setFunc( &MyHandler::key, this );
         slotMouse.setFunc( &MyHandler::mouse, this );

         vDataRoot_str = _dataRoot;
         vAlpha        = 1;
         vCurrentRot   = 0;

         vObject1.addData( vDataRoot_str + "test1.obj" );
         vObject1.addShader( vDataRoot_str + "shaders/triangle1" );
      }
      ~MyHandler();
      void windowClose( iEventInfo info ) {
         iLOG "User closed window" END
         info.iInitPointer->closeWindow();
      }
      void key( iEventInfo info );
      void mouse( iEventInfo info );
      void resize( iEventInfo info ) {
         iLOG "Window resized: W = " ADD info.eResize.width ADD ";  H = " ADD info.eResize.height END
         updateViewPort( 0, 0, GlobConf.win.width, GlobConf.win.height );
         calculateProjectionPerspective( GlobConf.win.width, GlobConf.win.height, 0.1, 100.0, 35.0 );
         vObject1.updateFinalMatrix();
         vObject1.updateUniforms();
      }

      int initGL();

      virtual void renderFrame() {vObject1.updateFinalMatrix(); vObject1.render();}


      _SLOT_ *getSWindowClose() {return &slotWindowClose;}
      _SLOT_ *getSResize()      {return &slotResize;}
      _SLOT_ *getSKey()         {return &slotKey;}
      _SLOT_ *getSMouse()       {return &slotMouse;}
};


#endif // HANDLER_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
