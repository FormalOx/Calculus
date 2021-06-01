#include <iostream>

using namespace std;

class Pump {
  float p1, p2, p3;
  float deltaX;
  
  const float   minFlow = 250;
  float maxFlow;
  
  float production;
  
  bool  active;
  
  public :
  
  int id;
  
  void SetDelta( float a1 ) { deltaX = a1; production = GetPartialProduction(); }
  
  float GetPartialProduction() {
    if( !active ) return 0;
    return  -p1 + p2 * deltaX - p3 * deltaX * deltaX * 1 / 100000;
  }
  
  float GetPartialProduction( float a1 ) {
    SetDelta( a1 );
    return GetPartialProduction();
  }
  
  void SetActive( bool a1 ) {
    active = a1;
  }
  
  bool GetActive() {
    return active;
  }
  
  Pump() {}
  Pump( int aID, float ap1, float ap2, float ap3, float aMx ) {
    id = aID; p1 = ap1; p2 = ap2; p3 = ap3; maxFlow = aMx;
  }
  Pump( const Pump& a1 ) { id = a1.id; active = a1.active; p1 = a1.p1; p2 = a1.p2; p3 = a1.p3; maxFlow = a1.maxFlow; deltaX = a1.deltaX; }
  Pump operator=(const Pump& a1 ) { id = a1.id; active = a1.active; p1 = a1.p1; p2 = a1.p2; p3 = a1.p3; maxFlow = a1.maxFlow; deltaX = a1.deltaX; return *this; }
};

class Station {
  Pump  p1, p2, p3;
  float total;
  
  public : 
  
  Station( int aT = 0 ) {
    p1 = Pump( 1, 18.89f, 0.1277f, 4.08f, 1110 );
    p2 = Pump( 2, 24.51f, 0.1358f, 4.69f, 1110 );
    p3 = Pump( 3, 27.02f, 0.138f, 3.84f, 1255 );
    total = aT;
  }
  
  void SetPumps( bool a1, bool a2, bool a3 ) {
    p1.SetActive( a1 ); 
    p2.SetActive( a2 ); 
    p3.SetActive( a3 ); 
  } 
  
  void SetTotal ( float a1 ) {
    total = a1;
  }
  
  void SetDelta ( float a1, float a2, float a3 ) {
    if( a1 + a2 + a3 > total ) { return; }
    p1.SetDelta( a1 );
    p2.SetDelta( a2 );
    p3.SetDelta( a3 );
  }
  
  float GetProduction() {
    return ( p1.GetPartialProduction() + p2.GetPartialProduction() + p3.GetPartialProduction() ) * ( 170 - ( 1.6f * total ) / 1000000 );
  }
  float GetProduction( float a1, float a2, float a3 ) {
    SetDelta( a1, a2, a3 );
    return GetProduction();
  }
};

int main() {
  Station core;
  core.SetTotal( 2500 );
  core.SetPumps( true, true, true );
  float delta, mx = 0;
  for( int i = 250; i < 1110; i++ ) {
    for( int j = 250; j < 1110 && i + j < 2250; j++ ) {
      for( int k = 250; k < 1255 && k + j + i < 2500; k++ ) {
        delta = core.GetProduction( i, j, k );
        if( mx < delta ) {
          mx = delta;
          cout<<i<<" "<<j<<" "<<k<<" "<<delta<<"\n";
        }
      }
    }
  }
}
