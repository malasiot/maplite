#!/bin/bash

wget -O osm/greece.pbf http://download.geofabrik.de/europe/greece-latest.osm.pbf

# Κεντρική Μακεδονία
osmosis --read-pbf osm/greece.pbf --bb left=21.6540 top=41.6154 bottom=39.8592 right=24.5160 completeWays=yes completeRelations=yes --write-pbf osm/makedonia.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_makedonia.shp land-polygons-split-4326/land_polygons.shp -clipsrc 21.6540 39.8592 24.5160 41.6154
phyghtmap -a 21.6540:39.8592:24.5160:41.6154 -s 20 -0 -o contours/makedonia/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_makedonia.shp --bbox "21.6540 39.8592 24.5160 41.6154" --out osm/makedonia.map --map-start-position "40.9741 22.6103" --map-start-zoom 14  osm/makedonia.pbf contours/makedonia/*.pbf

# Ανατολική Μακεδονία & Θράκη
osmosis --read-pbf osm/greece.pbf --bb left=24.4226 top=41.7836 bottom=40.2753 right=26.6913 completeWays=yes completeRelations=yes --write-pbf osm/thraki.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_thraki.shp land-polygons-split-4326/land_polygons.shp -clipsrc 24.4226 40.2753 26.6913 41.7836
phyghtmap -a 24.4226:40.2753:26.6913:41.7836 -s 20 -0 -o contours/thraki/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_thraki.shp --bbox "24.4226 40.2753 26.6913 41.7836" --out osm/thraki.map --map-start-position "41.1746 25.3345" --map-start-zoom 14  osm/thraki.pbf contours/thraki/*.pbf

# Δυτική Μακεδονία & Ήπειρος
osmosis --read-pbf osm/greece.pbf --bb left=19.2096 top=40.9612 bottom=39.1002 right=21.8079 completeWays=yes completeRelations=yes --write-pbf osm/west-makedonia-epirus.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_west-makedonia-epirus.shp land-polygons-split-4326/land_polygons.shp -clipsrc 19.2096 39.1002 21.8079 40.9612
phyghtmap -a 19.2096:39.1002:21.8079:40.9612 -s 20 -0 -o contours/west-makedonia-epirus/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_west-makedonia-epirus.shp --bbox "19.2096 39.1002 21.8079 40.9612" --out osm/west-makedonia-epirus.map --map-start-position "40.3403 20.9207" --map-start-zoom 14  osm/west-makedonia-epirus.pbf contours/west-makedonia-epirus/*.pbf

# Πελοπόνησσος
osmosis --read-pbf osm/greece.pbf --bb left=20.2148 top=38.3330 bottom=36.0225 right=23.4009 completeWays=yes completeRelations=yes --write-pbf osm/peloponese.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_peloponese.shp land-polygons-split-4326/land_polygons.shp -clipsrc 20.2148 36.0225 23.4009 38.3330
phyghtmap -a 20.2148:36.0225:23.4009:38.3330 -s 20 -0 -o contours/peloponese/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_peloponese.shp --bbox "20.2148 36.0225 23.4009 38.3330" --out osm/peloponese.map --map-start-position "37.3751 21.8156" --map-start-zoom 14  osm/peloponese.pbf contours/peloponese/*.pbf

# Θεσσαλία
osmosis --read-pbf osm/greece.pbf --bb left=21.6650 top=39.9519 bottom=39.0448 right=24.3347 completeWays=yes completeRelations=yes --write-pbf osm/thessaly.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_thessaly.shp land-polygons-split-4326/land_polygons.shp -clipsrc 21.6650 39.0448 24.3347 39.9519
phyghtmap -a 21.6650:39.0448:24.3347:39.9519 -s 20 -0 -o contours/thessaly/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_thessaly.shp --bbox "21.6650 39.0448 24.3347 39.9519" --out osm/thessaly.map --map-start-position "39.5848 22.7395" --map-start-zoom 14  osm/thessaly.pbf contours/thessaly/*.pbf

# Στερεά Ελλάδα
osmosis --read-pbf osm/greece.pbf --bb left=20.2739 top=39.1215 bottom=38.2091 right=22.6263 completeWays=yes completeRelations=yes --write-pbf osm/sterea.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_sterea.shp land-polygons-split-4326/land_polygons.shp -clipsrc 20.2739 38.2091 22.6263 39.1215
phyghtmap -a 20.2739:38.2091:22.6263:39.1215 -s 20 -0 -o contours/sterea/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_sterea.shp --bbox "20.2739 38.2091 22.6263 39.1215" --out osm/sterea.map --map-start-position "38.7566 21.2270" --map-start-zoom 14  osm/sterea.pbf contours/sterea/*.pbf

# Αττική & Ευβοια
osmosis --read-pbf osm/greece.pbf --bb left=22.5165 top=39.0725 bottom=37.1778 right=24.6753 completeWays=yes completeRelations=yes --write-pbf osm/attika.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_attika.shp land-polygons-split-4326/land_polygons.shp -clipsrc 22.5165 37.1778 24.6753 39.0725
phyghtmap -a 22.5165:37.1778:24.6753:39.0725 -s 20 -0 -o contours/attika/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_attika.shp --bbox "22.5165 37.1778 24.6753 39.0725" --out osm/attika.map --map-start-position "37.9370 23.8079" --map-start-zoom 14  osm/attika.pbf contours/attika/*.pbf

# Κρήτη, Νότο Αιγαίο
osmosis --read-pbf osm/greece.pbf --bb left=23.0493 top=37.3003 bottom=34.5066 right=28.5864 completeWays=yes completeRelations=yes --write-pbf osm/crete.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_crete.shp land-polygons-split-4326/land_polygons.shp -clipsrc 23.0493 34.5066 28.5864 37.3003
phyghtmap -a 23.0493:34.5066:28.5864:37.3003 -s 20 -0 -o contours/crete/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_crete.shp --bbox "23.0493 34.5066 28.5864 37.3003" --out osm/crete.map --map-start-position "36.0040 24.8968" --map-start-zoom 14  osm/crete.pbf contours/crete/*.pbf

# Βόρειο Αιγαίο
osmosis --read-pbf osm/greece.pbf --bb left=24.0710 top=40.3717 bottom=37.0552 right=27.4988 completeWays=yes completeRelations=yes --write-pbf osm/aegean.pbf
 ogr2ogr -overwrite -f "ESRI Shapefile" land-polygons-split-4326/land_polygons_aegean.shp land-polygons-split-4326/land_polygons.shp -clipsrc 24.0710 37.0552 27.4988 40.3717
phyghtmap -a 24.0710:37.0552:27.4988:40.3717 -s 20 -0 -o contours/aegean/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3
/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_aegean.shp --bbox "24.0710 37.0552 27.4988 40.3717" --out osm/aegean.map --map-start-position "39.0403 25.4443" --map-start-zoom 14  osm/aegean.pbf contours/aegean/*.pbf

