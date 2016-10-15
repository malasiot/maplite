
#include "MapFile.h"

#include <fstream>

#include "XmlDocument.h"

using namespace std ;


int main(int argc, char *argv[])
{

     MapFile f ;

    f.open("/tmp/osm.sqlite") ;

    BBox bbox(23.6045, 40.40544, 23.7755, 40.813799) ;

    vector<POI> pois ;
  //  f.queryPOIs("osm_pois", pois, "", "Ανά", 0, 200000, 22.56, 35.31 ) ;
      f.queryPOIs("osm_pois", pois, "", "Ανά*", 0, 10000, 25.56, 35.31 ) ;

    for(int i=0 ; i<pois.size() ; i++ )
        cout << pois[i].distance_ << endl ;


    cout << "ok" ;

}

