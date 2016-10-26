This is an implementation of a slippy map tile server that provides tiles from one or more Mapsforge maps.
It is based on boost::asio library. 

The server expects a configuration file which has the form:
```
<?xml version="1.0" encoding="UTF-8"?>
<config>
	<tiles key="..." src="..." theme="..." type="..." layer="..." />
	...
	<assets key="..." src="..."
</config>
```
The attribution key is prepended to the request url path. It is also used to create folder for caching tiles so it should not contain any special characters which are invalid for url or filesystem. 
The type can be either "mapsforge" or "jpeg2000". The attribute "src" is the relative path of the map file. For Mapsforge you should also define "theme" the relative path to the theme file and 
optionally layer (otherwise the default is used).
The tiles element configures an endpoint of the form 
```
http://<host>:<port>/<key>/<z>/<x>/<y>.png
```
The assets elements provide the end-point:
```
http://<host>:<port>/<key>/*
```
The path after the key is appended to the "src" to determine the relative path to an asset e.g. style files, or map overlays. Assets maybe collected inside an
SQLite database (see tools/make_res.cpp). For the case of GPX, KML assets the server supports conversion to GeoJson by appending ?cnv=geojson to the request URL.
All paths are relative to the path of the configuration file.
