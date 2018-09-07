from make_script import OSMImporter
import sys, os
import argparse
import xml.sax

def main(out_dir):
  	source = open('maps.osm')
  	handler = OSMImporter()
  	xml.sax.parse(source, handler)
  	handler.makeMaps(out_dir)

#  	print handler.segments
 
if __name__ == "__main__":
	arg_parser = argparse.ArgumentParser()
	arg_parser.add_argument("dir", help="data dir")
	args = arg_parser.parse_args()

	main(args.dir)
