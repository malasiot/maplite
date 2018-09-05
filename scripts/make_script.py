import xml.sax
import argparse
from lxml import etree as ET
import sys

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
					map_segment['name'] = tags['name'] ;
					map_segment['bbox'] = bbox['box'] ;
					map_segment['centroid'] = bbox['centroid'] ;
					self.maps.append(map_segment);
					print map_segment ;
	def makeScript(self, outFile):
		for seg in self.maps:
			cmd = "osmosis --read-pbf osm/greece.pbf --bb left=%2.4f top=%2.4f bottom=%2.4f right=%2.4f completeWays=yes completeRelations=yes --write-pbf osm/%s.pbf" % ( seg['bbox'][1], seg['bbox'][2], seg['bbox'][0], seg['bbox'][3], seg['id'])
			print cmd 
			cmd = "--filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_{0}.shp --bbox \"{1:2.4f} {2:2.4f} {3:2.4f} {4:2.4f}\" --out osm/{0}.map --map-start-position \"{5:2.4f} {6:2.4f}\" --map-start-zoom 14  osm/{0}.pbf".format(seg['id'], seg['bbox'][1], seg['bbox'][0], seg['bbox'][3], seg['bbox'][2], seg['centroid'][0], seg['centroid'][1])
			print cmd
			

 
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
