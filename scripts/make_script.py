import xml.sax
import argparse
from lxml import etree as ET
import sys, os, codecs
import datetime
import time
import shutil
import zipfile
import hashlib

downloadUrl = "https://vision.iti.gr/hellaspath/data/maps/" ;

def md5(fileName):
	digest = hashlib.md5()
	buf_size = 65536
	with open(fileName, 'rb') as f:
		data = f.read(buf_size)
		while data:
			digest.update(data)
			data = f.read(buf_size)
	
	return digest.hexdigest()

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

def getPolygon(map_extract, coords):
	
	poly = []
	count = 0 ;
	for ref in map_extract['refs']:
		latlon = coords[ref]
		poly.append(latlon)

	return poly ;

def writePolygon(fileName, coords):
	file = open(fileName, "w+") 
	file.write(fileName)
	file.write('\n') ;
	file.write("poly\n")
	for latlon in coords:
		file.write("   {0} {1}\n".format(latlon[1], latlon[0]) )
	file.write("END\n") ;
	file.write("END") ;
	file.close()

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
					map_segment['polygon'] = getPolygon(self.current, self.coords);
					self.maps.append(map_segment);
	def makeScript(self, dataDir):
		ofile = codecs.open('make_maps.sh', 'w', "utf-8") 
		ofile.write("#!/bin/bash\n\n")
		ofile.write("wget -O osm/greece-latest.pbf http://download.geofabrik.de/europe/greece-latest.osm.pbf\n\n")
		
		st = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d %H:%M:%S')
		
		for seg in self.maps:
			polyName = "osm/{0}.poly".format(seg['id'])
			writePolygon(polyName, seg['polygon'])
			ofile.write(u'# {0}\n'.format(seg['title']))
#			cmd = "osmosis --read-pbf osm/greece-latest.pbf --bb left=%2.4f top=%2.4f bottom=%2.4f right=%2.4f completeWays=yes completeRelations=yes --write-pbf osm/%s.pbf\n" % ( seg['bbox'][1], seg['bbox'][2], seg['bbox'][0], seg['bbox'][3], seg['id'])
			cmd = 'osmosis --read-pbf osm/greece-latest.pbf --bounding-polygon file="%s" completeWays=yes completeRelations=yes --write-pbf osm/%s.pbf\n' % ( polyName, seg['id'])
			ofile.write(cmd) ;
			cmd = " ogr2ogr -overwrite -f \"ESRI Shapefile\" land-polygons-split-4326/land_polygons_{0}.shp land-polygons-split-4326/land_polygons.shp -clipsrc {1:2.4f} {2:2.4f} {3:2.4f} {4:2.4f}\n".format(seg['id'], seg['bbox'][1], seg['bbox'][0], seg['bbox'][3], seg['bbox'][2])
			ofile.write(cmd) ;
			cntr_path = "contours/{0}/contours".format(seg['id']) ;
			if not os.path.exists(cntr_path):
				os.makedirs(cntr_path)
#			cmd = "phyghtmap -a {1:2.4f}:{2:2.4f}:{3:2.4f}:{4:2.4f} -s 20 -0 -o contours/{0}/contours -c 1000,100 --start-node-id=1000000000 --start-way-id=1000000000 --pbf --source=view3\n".format(seg['id'], seg['bbox'][1], seg['bbox'][0], seg['bbox'][3], seg['bbox'][2])
			cmd = 'mkdir -p contours/{0}/\n'.format(seg['id'])
			ofile.write(cmd) ;
			cmd = 'phyghtmap --polygon="{1}" -s 20 -0 -o contours/{0}/contours -c 1000,100 --start-node-id=20000000000 --start-way-id=20000000000 --pbf --source=srtm1 --srtm-version=3.0 --earthexplorer-user=malasiot --earthexplorer-password="5&|a?GfF%Tu&TXV"\n'.format(seg['id'], polyName)
			ofile.write(cmd) ;
			cmd = "/home/malasiot/source/maplite/build/src/convert/osm2map --filter filter.cfg --land-polygon land-polygons-split-4326/land_polygons_{0}.shp --bbox \"{1:2.4f} {2:2.4f} {3:2.4f} {4:2.4f}\" --out osm/{0}.map --map-start-position \"{5:2.4f} {6:2.4f}\" --date \"{7}\" --map-start-zoom 14  osm/{0}.pbf mountains.osm contours/{0}/*srtm1v3.0.osm.pbf\n".format(seg['id'], seg['bbox'][1], seg['bbox'][0], seg['bbox'][3], seg['bbox'][2], seg['centroid'][0], seg['centroid'][1], st)
			ofile.write(cmd) ;
			ofile.write('\n') ;

	def makeMaps(self, outDir):
		root = ET.Element("maps")
		for seg in self.maps:
			outFile = outDir + "/maps/" + seg['id'] + ".zip"
			inFile = 'osm/' + seg['id'] + ".map"
			zf = zipfile.ZipFile(outFile, "w", zipfile.ZIP_DEFLATED)
			zf.write(inFile, seg['id'] + ".map")
			zf.close() ;
#			shutil.copyfile(inFile, outFile)
			fileSize = os.path.getsize(outFile) >> 20
			emap = ET.SubElement(root, "MapsforgeTileSource", attrib={"id": seg['id']})
			print(outFile)
			md5hash = md5(outFile)
#			md5hash = hashlib.md5(open(outFile,'rb').read()).hexdigest()
			print(md5hash)
			hashElement = ET.SubElement(emap, "hash")
			hashElement.text = md5hash
			hashElement.tail = '\n'
			downloadUrlElement = ET.SubElement(emap, "downloadUrl") ;
			downloadUrlElement.text = downloadUrl + seg['id'] + ".zip" ;
			localFileName = ET.SubElement(emap, "localFileName") ;
			localFileName.text = "maps/hellaspath/" + seg['id'] + ".map";
			downloadFileName = ET.SubElement(emap, "downloadFileName") ;
			downloadFileName.text = seg['id'] + ".zip";
			unzipFolder = ET.SubElement(emap, "unzipFolder") ;
			unzipFolder.text = "maps/hellaspath/" ;
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
