wget -O osm/greece.pbf http://download.geofabrik.de/europe/greece-latest.osm.pbf
osmosis --read-pbf osm/greece.pbf --bb left=21.654 top=41.6 bottom=39.859 right=24.51 completeWays=yes completeRelations=yes--write-pbf osm/makedonia.pbf
ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_makedonia.shp land-polygons-split-4326/land_polygons.shp -clipsrc 21.654 39.859 24.51 41.6
BuildMbr(1252344.27, 5009377.08, 2504688.5, 6261721.3, 3857)
