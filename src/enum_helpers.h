#ifndef ENUM_HELPERS_H
std::string to_string ( UberCasino::player_action_t t )
{
   std::string retval;
   switch ( t )
   {
      case (idle):retval="idle";break;
      case (hitting):retval="hitting";break; 
      case (standing):retval="standing";break;
      case (splitting):retval="splitting";break;
      case (doubling):retval="doubling";break;
   }
   return retval;
}
#define ENUM_HELPERS_H
#endif
