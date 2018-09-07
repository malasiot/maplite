import xml.sax
import argparse
from lxml import etree as ET
import sys, os, codecs
import datetime
import shutil

def getBoundingBox(map_extract, coords):
	
	min_lat = min_lon = float("inf")
	max_lat = max_lon = -float("inf")
	centroid = [0.0, 0.0]
	count = 0 ;
	for ref in map_extract['refs']:
		latlon = coords[ref]
		centroid[0] += latlon[0] ;
		centroid[1] += latlon[1] ;
		count = count + 1 ;
		min_lat = min(min_lat, float(latlon[0]))
		min_lon = min(min_lon, float(latlon[1]))
		max_lat = max(max_lat, float(latlon[0]))
		max_lon = max(max_lon, float(latlon[1]))
	centroid[0] /= count ;
	centroid[1] /= count ;
	return { "box": [min_lat, min_lon, max_lat, max_lon], "centroid": centroid }	

class OSMImporter(xml.sax.ContentHandler):
	def __init__(self):
		xml.sax.ContentHandler.__init__(self)
		self.maps = []
		self.coords = {}
		
	def startElement(self, name, attrs):
		if name == "node":
			self.current = {}
			self.current['osm_id'] = attrs.getValue('id')
			self.current['lat'] = lat = float(attrs.getValue('lat'))
			self.current['lon'] = lon = float(attrs.getValue('lon'))
			self.current['tags'] = {}
			self.coords[attrs.getValue('id')] = (lat, lon) ; 
		elif name == "way":
			self.current = {}
			self.current['osm_id'] = attrs.getValue('id')
			self.current['tags'] = {}
			self.current['refs'] = []
		elif name == "tag":
			k = attrs.getValue('k')
			v = attrs.getValue('v')
			self.current['tags'].update({ k: v }) ;
		elif name == "nd":
			osm_id = attrs.getValue('ref') ;
			self.current['refs'].append(osm_id) ;
	def endElement(self, name):
		if name in ["node", "way"]:
			if self.current['tags']:
				tags = self.current['tags']
				if name == "way":
					bbox = getBoundingBox(self.current, self.coords) ;
					map_segment = {}
					map_segment['id'] = tags['id'] ;
					map_segment['title'] = tags['title'] ;
					map_segment['desc'] = tags['desc'] ;
					map_segment['bbox'] = bbox['box'] ;
					map_segment['centroid'] = bbox['centroid'] ;
					self.maps.append(map_segment);
	def makeScript(self, dataDir):
		ofile = codecs.open('make_maps.sh', 'w', "utf-8") 
		ofile.write("#!/bin/bash\n\n")
		ofile.write("wget -O osm/greece.pbf http://download.geofabrik.de/europe/greece-latest.osm.pbf\n\n")
		for seg in self.maps:
			ofile.write(u'# {0}\n'.format(seg['title']))
			cmd = "osmosis --read-pbf osm/greece.pbf --bb left=%2.4f top=%2.4f bottom=%2.4f right=%2.4f completeWays=yes completeRelations=yes --write-pbf osm/%s.pbf\n" % ( seg['bbox'][1], seg['bbox'][2], seg['bbox'][0], seg['bbox'][3], seg['id'])
			ofile.write(cmd) ;
			cmd = " ogr2ogr -overwrite -f \"ESRI Shapefile\" land-polygons-split-4326/land_polygons_{0}.shp land-polygons-split-4326/land_polygons.shp -clipsrc {1:2.4f} {2:2.4f} {3:2.4f} {4:2.4f}\n".format(seg['id'], seg['bbox'][1], seg['bbox'][0], seg['bbox'][3], seg['bbox'][2])
			ofile.write(cmd) ;
			cntr_path = "contours/{0}/contours".format(seg['id']) ;
			if not os.path.exists(cntr_path):
				os.makedirs(cntr_path)
			cmd = "phyghtmap -a {1:2.4f}:{2:2.4f}:{3:2.4f}:{4:2.4f} -s 20 -0 -o contours/{0}/contours -c 1000,100 --start-node-id=10000000000 --pbf --source=view3\n".format(seg['id'], seg['bbox'][1], seg['bbox'][0], seg['bbox'][3], seg['bbox'][2])
			ofile.write(cmd) ;
			cmd = "/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_{0}.shp --bbox \"{1:2.4f} {2:2.4f} {3:2.4f} {4:2.4f}\" --out osm/{0}.map --map-start-position \"{5:2.4f} {6:2.4f}\" --map-start-zoom 14  osm/{0}.pbf contours/{0}/*.pbf\n".format(seg['id'], seg['bbox'][1], seg['bbox'][0], seg['bbox'][3], seg['bbox'][2], seg['centroid'][0], seg['centroid'][1])
			ofile.write(cmd) ;
			ofile.write('\n') ;

	def makeMaps(self, outDir):
		root = ET.Element("maps")
		for seg in self.maps:
			outFile = outDir + "/maps/" + seg['id'] + ".map"
			inFile = 'osm/' + seg['id'] + ".map"
			shutil.copyfile(inFile, outFile)
			fileSize = os.path.getsize(outFile) >> 20
			emap = ET.SubElement(root, "MapsforgeTileSource", attrib={"id": seg['id']})
			downloadUrl = ET.SubElement(emap, "downloadUrl") ;
			downloadUrl.text = "https://vision.iti.gr/trails/data/hellaspath/maps/" + seg['id'] + ".map" ;
			localFileName = ET.SubElement(emap, "localFileName") ;
			localFileName.text = "maps/hellaspath/" + seg['id'] + ".map";
			title = ET.SubElement(emap, "title") ;
			title.text = seg['title'] ;
			desc = ET.SubElement(emap, "description") ;
			desc.text = seg['desc'] + " ({0:d}MB) ".format(fileSize) ;
			attribution = ET.SubElement(emap, "attribution") ;
			attribution.text = "Openstreetmap Data" ;
			version = ET.SubElement(emap, "version") ;
			today = datetime.date.today()
			version.text = today.strftime('%Y-%m-%d') ;
		et = ET.ElementTree(root)
		et.write(outDir + "/offline_maps.xml", pretty_print=True, xml_declaration=True, encoding='UTF-8')	

 
def main(data_dir):
  	source = open(data_dir + '/maps.osm')
  	handler = OSMImporter()
  	xml.sax.parse(source, handler)
  	handler.makeScript(data_dir + '/script.sh')
  	
	

#  	print handler.segments
 
if __name__ == "__main__":
	arg_parser = argparse.ArgumentParser()
	arg_parser.add_argument("dir", help="data dir")
	args = arg_parser.parse_args()

	main(args.dir)
