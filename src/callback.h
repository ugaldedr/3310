


// PTR is a global.  Horrible hack

void _cb ( UberCasino::Player P )
{
#ifdef DEBUG_PRINT
  std::cout << "\nRECEIVED" ;
#endif
  PTR->external_data ( P );
}
void _cb ( UberCasino::Dealer D )
{
#ifdef DEBUG_PRINT
  std::cout << "\nRECEIVED -- Dealer" << std::endl;
  boost::uuids::uuid u;
  memcpy(&u, D.uuid, 16);
  std::cout << "   uuid " << boost::uuids::to_string( u ) << std::endl;
  std::cout << "   name " << D.name << std::endl;
  memcpy(&u, D.game_uuid, 16);
  std::cout << "   game_uuid " << boost::uuids::to_string( u ) << std::endl;
#endif
  PTR->external_data ( D );
}
void _cb ( UberCasino::Game G )
{
#ifdef DEBUG_PRINT
#endif
  PTR->external_data ( G );
}

