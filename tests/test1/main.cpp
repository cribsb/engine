
#include "config.hpp"
#include "handler.hpp"
#include "cmdANDinit.hpp"
#include <time.h>
#include <engine.hpp>

#if WINDOWS
#include <windows.h>
#endif

using namespace std;
using namespace e_engine;

#define DO_SHA      0
#define OLD_RENDER  0
#define TEST_SHADER 0

void hexPrint( std::vector<unsigned char> const &_v ) {
   for( unsigned char const & c : _v )
      printf( "%02X ", c );
   printf( "\n\n" );
   fflush( stdout );
}

// #undef  UNIX
// #define UNIX 0


int main( int argc, char *argv[] ) {
   cmdANDinit cmd( argc, argv );

   if( ! cmd.parseArgsAndInit() ) {
      B_SLEEP( seconds, 1 );
      return 1;
   }

#if OLD_RENDER
   uRandomISAAC myRand;

   const int ValChange = 50;

   r = 0;
   g = 0;
   b = 0;

   R = ( ( float ) myRand( 10, ValChange ) / 5000 );
   G = ( ( float ) myRand( 10, ValChange ) / 5000 );
   B = ( ( float ) myRand( 10, ValChange ) / 5000 );

   rr = myRand( 0, 1 ) ? true : false;
   gg = myRand( 0, 1 ) ? true : false;
   bb = myRand( 0, 1 ) ? true : false;
#endif


   iLOG( "User Name:     ", SYSTEM.getUserName()          );
   iLOG( "User Login:    ", SYSTEM.getUserLogin()         );
   iLOG( "Home:          ", SYSTEM.getUserHomeDirectory() );
   iLOG( "Main config:   ", SYSTEM.getMainConfigDirPath() );
   iLOG( "Log File Path: ", SYSTEM.getLogFilePath()       );

   iInit start;

   if( start.init() == 1 ) {
      MyHandler handler( cmd.getDataRoot(), cmd.getMesh(), &start );
      start.addWindowCloseSlot( handler.getSWindowClose() );
      start.addResizeSlot( handler.getSResize() );
      start.addKeySlot( handler.getSKey() );
      start.addMousuSlot( handler.getSMouse() );
      start.addFocusSlot( start.getAdvancedGrabControlSlot() );

#if 0
      vector<iDisplays> displays = start.getDisplayResolutions();

      iLOG( "Displays: ", displays.size() );

      for( GLuint i = 0; i < displays.size(); ++i ) {
         iLOG( "Display ", i, ": ", displays[i].getName() );
      }

      if( displays.size() == 2 ) {
//          displays[0].disable();
//          displays[1].disable();

//          iLOG( start.setDisplaySizes( displays[0] ) );
//          iLOG( start.setDisplaySizes( displays[1] ) );

//          start.applyNewRandRSettings();

//          B_SLEEP( seconds, 1 );

//          displays.clear();
//          displays = start.getDisplayResolutions();

         displays[0].enable();
         displays[1].enable();
         displays[0].autoSelectBest();
         displays[1].autoSelectBest();
//          iLOG( start.setDisplaySizes( displays[0] ) );
//          iLOG( start.setDisplaySizes( displays[1] ) );
//          start.applyNewRandRSettings();

//          displays.clear();
//          displays = start.getDisplayResolutions();

         displays[0].setNoClones();
         displays[1].setNoClones();
         displays[1].setPositionAbsolute( 0, 0 );
         displays[0].setPositionRelative( iDisplays::RIGHT_OFF, displays[1] );
         iLOG( start.setDisplaySizes( displays[0] ) );
         iLOG( start.setDisplaySizes( displays[1] ) );
         start.setPrimary( displays[1] );
         start.applyNewRandRSettings();
         start.setPrimary( displays[1] );
      }
#endif

#if TEST_SHADER
      string temp;
      temp += cmd.getDataRoot() + "shaders/colors_p";

      rShader prog( temp );
      GLuint dummy;
      prog.compile( dummy );
#endif

      if( handler.initGL() == 1 )
         start.startMainLoop();

      start.closeWindow();
   }

#if DO_SHA == 1
   uSHA_2 mySHA( SHA2_384 );
   mySHA.selftest();
#endif


//    iLOG( "Credits: "
//    POINT "Daniel ( Mense ) Mensinger"
//    POINT "Dennis Schunder"
//    POINT "Silas Bartel"
//    );

//    B_SLEEP( seconds, 1 );

//    B_SLEEP( seconds, 1 );

   start.shutdown();

   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 



