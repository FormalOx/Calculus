#include <iostream>

using namespace std;

struct Pump {
  float p1, p2, p3;
  float deltaX;
  
  const float   minFlow = 250;
  float maxFlow;
  
  float production;
  
  bool  active;
  
  float GetPartialProduction() {
    if( !active || deltaX < minFlow || deltaX > maxFlow ) return 0;
    return  -p1 + p2 * deltaX - ( p3 * deltaX * deltaX ) / 100000;
  }

  bool IsOutOfRange() {
    if( !active ) return false;
    if( deltaX > maxFlow || deltaX < minFlow ) return true;
    return false;
  }

  Pump() {}
  Pump( float ap1, float ap2, float ap3, float aMx ) {
    p1 = ap1; p2 = ap2; p3 = ap3; maxFlow = aMx; active = true;
  }
  Pump( const Pump& a1 ) { active = a1.active; p1 = a1.p1; p2 = a1.p2; p3 = a1.p3; maxFlow = a1.maxFlow; deltaX = a1.deltaX; }
  Pump operator=(const Pump& a1 ) { active = a1.active; p1 = a1.p1; p2 = a1.p2; p3 = a1.p3; maxFlow = a1.maxFlow; deltaX = a1.deltaX; return *this; }
};

struct Station {
  Pump  p1, p2, p3;
  float total;
  
  const float p11 = 18.89f, p12 = 24.51f, p13 = 27.02f;
  const float p21 = 0.1277f, p22 = 0.1358f, p23 = 0.138f;
  const float p31 = 4.08f, p32 = 4.69f, p33 = 3.84f;
  
  Station( int aT = 0 ) {
    
    p1 = Pump( p11, p21, p31, 1110 );
    p2 = Pump( p12, p22, p32, 1110 );
    p3 = Pump( p13, p23, p33, 1255 );
    
    total = aT;
  }
  
  float GetProduction() {
    return ( p1.GetPartialProduction() + p2.GetPartialProduction() + p3.GetPartialProduction() ) * ( 170.0f - ( 1.6f * total ) / 1000000.0f );
  }
  

  float GetLambdaDerived ( float ap2, float ap3, float lambda ) {    //lambda based calculator ( all x, y, z separated so it works on all cases )
    return 100000.0 * ( lambda + ap2 ) / ( 2 * ap3 );
  }
  
  /*
  float GetFlows ( float total ) {    //DEPRECATED
  
    float delta =  ( ( total / 100000.0 ) * ( 2 * p1.Gc3() * p2.Gc3() * p3.Gc3() ) - ( p1.Gc2v2() * p2.Gc3() * p3.Gc3() + p2.Gc2v2() * p1.Gc3() * p3.Gc3() + p3.Gc2v2() * p1.Gc3() * p2.Gc3() ) ) / ( p1.Gc3v2() * p2.Gc3v2() + p1.Gc3v2() * p3.Gc3v2() + p2.Gc3v2() * p3.Gc3v2() ); 
    
    cout<< total << delta << " " << GetLambda( p21, p31, delta ) << " " << GetLambda( p22, p32, delta ) << " " << GetLambda( p23, p33, delta ) <<endl;
  
    cout<<endl; 
    
    return 0;
  }
  */
  
  float Production ( bool a1, bool a2, bool a3 ) {
    
    if ( !a1 && !a2 && !a3 ) { return 0; }

    p1.active = a1;
    p2.active = a2;
    p3.active = a3;
    
    if( ( a1 && !a2 && !a3 ) || ( !a1 && a2 && !a3 ) || ( !a1 && !a2 && a3 ) ) {
      cout<<"FAULT : SINGLE PUMP USE ( " << ( a1 ? "P1" : "" ) << ( a2 ? "P2" : "" ) << ( a3 ? "P3" : "" ) << " ): NO NEED FOR CALCULATIONS\n";
      return -1;
    }
    
    float delta;
    if( a1 && a2 && a3 ) {
      delta = ( ( total / 100000.0 ) * ( 2 * p31 * p32 * p33 ) - ( p21 * p32 * p33 + p22 * p31 * p33 + p23 * p31 * p32 ) ) / ( p31 * p32 + p31 * p33 + p32 * p33 ); 
    } else {
      if( a1 ) {
        if( a2 ) { 
          delta = ( ( total / 100000.0 ) * ( 2 * p32 * p31 ) - ( p22 * p31 + p21 * p32 ) ) / ( p32 + p31 ); // 1 & 2 
        } else {
          delta = ( ( total / 100000.0 ) * ( 2 * p31 * p33 ) - ( p21 * p33 + p23 * p31 ) ) / ( p31 + p33 ); // 1 & 3 
        }
      } else {
        delta = ( ( total / 100000.0 ) * ( 2 * p32 * p33 ) - ( p22 * p33 + p23 * p32 ) ) / ( p32 + p33 ); // 2 & 3 
      }
    }
    
    p1.deltaX = GetLambdaDerived( p21, p31, delta );
    p2.deltaX = GetLambdaDerived( p22, p32, delta );
    p3.deltaX = GetLambdaDerived( p23, p33, delta );
  
    //cout<< ( a1 ? p1.deltaX : 0 ) <<" "<<( a2 ? p2.deltaX : 0 )<<" "<<( a3 ? p3.deltaX : 0 )<<endl;
  
    if ( p1.IsOutOfRange() || p2.IsOutOfRange() || p3.IsOutOfRange() ) { 
      //cout<<"FAULT : OUT OF RANGE : FIXATE ONE OR MORE PARAMETERS\n"; 
      return -1;
      //return Production( !p1.IsOutOfRange(), !p2.IsOutOfRange(), !p3.IsOutOfRange() ); 
    }

    p1.active = a1;
    p2.active = a2;
    p3.active = a3;
    
    return 1;
    return GetProduction();
  }

};

int main() {
  /*
   * HOW TO USE :
   * 
   * SET    core TOTAL to your total intake amout
   * RUN    core.Production ( true, true, true )
   * CHECK  if there is a fault, restrict the pumps ( i.e. if pump 3 requires more than 1255 then remove that amount from total and rerun production( true, true, false ) )
   * GOTO   run if there was a fault and you restricted the pumps
   * 
   * DONE
   */
  
  Station core = Station( 3400 );
  
  //cout<<core.Production( true, true, true );
  /*
  for( int i = 750; i < 2220 + 1255; i++ ) {
    core.total = i;
    if( core.Production( true, true, true ) == -1 ) {
      cout<<i<<" "<<core.Production( true, true, true )<<"\n";
    }
  }
  */
  
  return 0;
}
