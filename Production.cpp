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

  float GetPartialProduction( float a1 ) {
    float delta0 = deltaX;
    deltaX = a1;
    a1 = GetPartialProduction();
    deltaX = delta0;
    return a1;
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
    return ( p1.GetPartialProduction() + p2.GetPartialProduction() + p3.GetPartialProduction() ) * GetPartialProductionElement();
  }
  
  float GetPartialProductionElement() {
    return ( 170.0f - ( 1.6f * total ) / 1000000.0f );
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

    if( ( a1 && !a2 && !a3 ) || ( !a1 && a2 && !a3 ) || ( !a1 && !a2 && a3 ) ) {
      cout<<"FAULT : SINGLE PUMP USE ( " << ( a1 ? "P1" : "" ) << ( a2 ? "P2" : "" ) << ( a3 ? "P3" : "" ) << " ): NO NEED FOR CALCULATIONS\n";
      return GetProduction();
    }
    
    p1.active = a1;
    p2.active = a2;
    p3.active = a3;
    
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
    
    if( a1 ) p1.deltaX = GetLambdaDerived( p21, p31, delta );
    if( a2 ) p2.deltaX = GetLambdaDerived( p22, p32, delta );
    if( a3 ) p3.deltaX = GetLambdaDerived( p23, p33, delta );
  
    cout<< ( a1 ? p1.deltaX : 0 ) <<" "<<( a2 ? p2.deltaX : 0 )<<" "<<( a3 ? p3.deltaX : 0 )<<endl;
  
    if ( p1.IsOutOfRange() || p2.IsOutOfRange() || p3.IsOutOfRange() ) { 
      cout<<"FAULT : OUT OF RANGE : FIXATE ONE OR MORE PARAMETERS\n"; 
      return -1;
      //return Production( !p1.IsOutOfRange(), !p2.IsOutOfRange(), !p3.IsOutOfRange() ); 
    }

    return GetProduction();
  }

};

int main() {
  /*
   * HOW TO USE :
   * 
   * SET      core TOTAL to your total intake amout
   * RUN      core.Production ( true, true, true )
   * CHECK    if there is a fault, restrict the pumps ( i.e. if pump 3 requires more than 1255 then remove that amount from total and rerun production( true, true, false ) )
   * GOTO     run if there was a fault and you restricted the pumps
   * WARNING  set the total correctly at the end, because the output is based on the total as well
   * DONE
   */
  
  Station core = Station();
  
  /*
  
  //USAGE TEMPLATE 
  
  core.total = TOTAL HERE
  cout<<core.Production( true, true, true )<<endl;
  
  cout<<" ----------------- \n";
  */
  
  
  /*
  //1 & 2
  for( int i = 750; i < 2220 + 1255; i++ ) {
    core.total = i;
    if( core.Production( true, true, true ) == -1 ) {
      cout<<i<<" "<<core.Production( true, true, true )<<"\n";
    }
  }
  cout<<" ----------------- \n";
  */
  
  /*
   //3
  Station core = Station( 2500 );
  cout<<core.Production( true, true, true )<<endl;
  cout<<" ----------------- \n";
  */
   
  /*
  //4
  core.total = 600;
  cout << core.p1.GetPartialProduction( 600 ) * core.GetPartialProductionElement() << endl;
  cout << core.p2.GetPartialProduction( 600 ) * core.GetPartialProductionElement() << endl;
  cout << core.p3.GetPartialProduction( 600 ) * core.GetPartialProductionElement() << endl;
  cout<<"\n";
  core.total = 1000;
  cout << core.p1.GetPartialProduction( 1000 ) * core.GetPartialProductionElement() << endl;
  cout << core.p2.GetPartialProduction( 1000 ) * core.GetPartialProductionElement() << endl;
  cout << core.p3.GetPartialProduction( 1000 ) * core.GetPartialProductionElement() << endl;
  cout<<" ----------------- \n";
  */
  
  /*
  //5
  core.total = 1500;
  cout<<core.Production( true, true, true )<<endl;
  cout<<core.Production( false, true, true )<<endl;
  cout<<core.Production( true, false, true )<<endl;
  cout<<core.Production( true, true, false )<<endl;
  cout<<" ----------------- \n";
  */
  
  /*
  //6
  core.total = 3400;
  cout<<core.Production( true, true, true )<<endl; // FAULT HERE
  
  core.total = 3400 - core.p3.maxFlow;
  core.p3.deltaX = core.p3.maxFlow;
  
  cout<<core.Production( true, true, false )<<endl;
  
  core.p3.active = true;
  
  core.total = 3400;
  cout<<core.GetProduction()<<endl;
  cout<<" ----------------- \n";
  */
  
  return 0;
}
