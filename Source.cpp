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
    if( deltaX < minFlow || deltaX > maxFlow ) return 0;
    return  -p1 + p2 * deltaX - ( p3 * deltaX * deltaX ) / 100000;
  }
  
  float GetProductionAtA1( float a1 ) {
    float delta1 = deltaX;
    SetDelta( a1 );
    return GetPartialProduction();
    SetDelta( delta1 );
  }
  
  float GetRiseAtA1( float a1 ) {
    //if( a1 < 250 || a1 > maxFlow ) return -1;
    return p2 - ( 2.0 / 100000 ) * p3 * a1;
  }
  
  float GetRiseIntersection( Pump& a1 ) {
    return ( 100000 * ( p2 - a1.p2 ) ) / ( 2 * ( p3 - a1.p3 ) );
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

struct Station {
  Pump  p1, p2, p3;
  float total;
  
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
    return ( p1.GetPartialProduction() + p2.GetPartialProduction() + p3.GetPartialProduction() ) * ( 170.0f - ( 1.6f * total ) / 1000000.0f );
  }
  float GetProduction( float a1, float a2, float a3 ) {
    SetDelta( a1, a2, a3 );
    return GetProduction();
  }
};

float lbc ( float p2, float p3, float lambda ) {    //lambda based calculator ( all x, y, z separated so it works on all cases )
  return 100000.0 * ( lambda + p2 ) / ( 2 * p3 );
}

float lambdaaa ( float total ) {
  float p21 = 0.1277f, p22 = 0.1358f, p23 = 0.138f;
  float p31 = 4.08f, p32 = 4.69f, p33 = 3.84f;

  float delta = ( ( total / 100000.0 ) * ( 2 * p31 * p32 * p33 ) - ( p21 * p32 * p33 + p22 * p31 * p33 + p23 * p31 * p32 ) ) / ( p31 * p32 + p31 * p33 + p32 * p33 ); 
  
  cout<< delta << " " << lbc( p21, p31, delta ) << " " << lbc( p22, p32, delta ) << " " << lbc( p23, p33, delta ) <<endl;

  cout<<endl; 
  
  return 0;
}

int main() {
  Station core;
  core.SetPumps( true, true, true );
  
  if( false ) {
    float delta, mx = 0, total = 1500;
    int mxI, mxJ, mxK;
    
    core.SetTotal( total );
    
    for( int i = 250; i <= 1110; i++ ) {
      for( int j = 250; j <= 1110 && i + j <= total - 250; j++ ) {
        for( int k = 250; k <= 1255 && k + j + i <= total; k++ ) {
          delta = core.GetProduction( i, j, k );
          if( mx < delta ) {
            mx = delta;
            mxI = i;
            mxJ = j;
            mxK = k;
            cout<<i<<" "<<j<<" "<<k<<" "<<delta<<"\n";
          }
        }
      }
    }
    
    cout<<" ------------------------\n ";
    mx *= 100;
    for( float i = mxI - 1.0; i <= mxI + 1.0; i += 0.01f ) {
      for( float j = mxJ - 1.0; j <= mxJ + 1.0 && i + j <= total - 250; j += 0.01f ) {
        for( float k = mxK - 1.0; k <= mxK + 1.0 && k + j + i <= total; k += 0.01f ) {
          delta = core.GetProduction( i, j, k );
          if( mx < delta * 100 ) {
            mx = delta * 100;
            cout<<i<<" "<<j<<" "<<k<<" "<<delta<<"\n";
          }
        }
      }
    }
    
  }
  
  //Rough estimate for best @ 2500 total :
  // 777 763 960 @ 30186.7 output
  
  cout<<" ------------------------\n ";
  cout<<core.p1.GetRiseIntersection( core.p2 )<<endl; //  663.935
  cout<<core.p2.GetRiseIntersection( core.p3 )<<endl; // -129.411
  cout<<core.p1.GetRiseIntersection( core.p3 )<<endl; // -2145.83
  
  // Only p1 and p2 have a derivate intersection within the input domain ( @ 663.935 )
  
  cout<<" ------------------------\n ";
  cout<<core.p1.GetRiseAtA1( 600 )<<" "<<core.p1.GetRiseAtA1( 700 )<<"\n";
  cout<<core.p2.GetRiseAtA1( 600 )<<" "<<core.p2.GetRiseAtA1( 700 )<<"\n";
  cout<<core.p3.GetRiseAtA1( 600 )<<"\n";
  
  //The derivatives gives us the following hierarchy
  //      663
  //  p3   |    p3 
  //  p2   |    p1
  //  p1   |    p2
  
  cout<<" ------------------------\n ";
  cout<<core.p1.GetProductionAtA1( 1110 )<<"\n";
  cout<<core.p2.GetProductionAtA1( 1110 )<<"\n";
  cout<<core.p3.GetProductionAtA1( 1250 )<<"\n";
  
  //Max flow tests reveal the following hierarchy 
  // p3 > p1 > p2
  
  cout<<" ------------------------\n ";
  cout<<core.p1.GetProductionAtA1( 663.935 )<<"\n";
  cout<<core.p2.GetProductionAtA1( 663.935 )<<"\n";
  
  cout<<" ------------------------\n ";
  int p2gp1 = 0;
  for( int i = 250; i <= 1110; i++ ) {
    if ( core.p1.GetProductionAtA1( i ) < core.p3.GetProductionAtA1( i ) ) { cout<<i<<" "; break; } 
  }
  cout<<p2gp1<<endl;
  
  //The following tests reveal that p2 will always perform worse than p1, no matter the input flow
  cout<<" ------------------------\n ";
  lambdaaa( 1500 );
  //More tests point to the folowing output hierarchy 
  //      430       682
  //  p1   |    p1   |    p3
  //  p2   |    p3   |    p1
  //  p3   |    p2   |    p2
}
