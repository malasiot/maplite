This is a tool that takes as input one or more OpenStreetMap files and outputs an offline map in Mapsforge [format](https://github.com/mapsforge/mapsforge/blob/master/docs/Specification-Binary-Map-File.md)
It works by first importing all geometries into a spatialite database and then use the database to split geometry into tiles.
```
Usage: osm2map [options] <files>*
  --help                             produce help
  --filter path                      osm tag filter configuration file
  --land-polygon path                shapefile containg the land enclosing 
                                     polygon
  --out path                         output map file path
  --bbox <minx miny maxx maxy>       map bounding box
  --map-start-position <lat lon>     hint for coordinates of map center
  --map-start-zoom zoom              hint for map start zoom
  --preferred-languages arg          map prefered languages in ISO
  --comment arg                      a comment to write to the file
  --zoom-interval-conf arg           a list of base_zoom, min_zoom, max_zoom 
                                     for all levels of the map
  --bbox-enlargement arg             amount in meters to expand each tile when 
                                     adding geometries
  --debug [=arg(=1)]                 enable debug information in the map file
  --label-positions arg (=true)      enable or disable computation of label 
                                     positions for areas covering multiple 
                                     tiles
  --polygon-clipping arg (=true)     clip polygons to tile boundaries
  --way-clipping arg (=true)         clip ways to tile boundaries
  --simplification-factor arg (=2.5) geometry simplification factor as error in
                                     pixels at each zoom level
```
The parameters are almost the same with the Osmosis Mapsforge writer [plugin](https://github.com/mapsforge/mapsforge/blob/master/docs/Getting-Started-Map-Writer.md)
Input files can be in osm, osm.gz or PBF format. The main difference with Osmosis is tag filtering process that allows filtering 
based on complex  conditional expressions (see [filter.cfg](https://github.com/malasiot/mftools/blob/master/data/filter.cfg) for a translation of the default Osmosis tag filter), and offers the posibility to change the tag scheme 
completely in order to help rendering. The parameter --land-polygon points to a Shapefile containing the land boundary. 
If no --bbox is given it is computed automatically from input geometries. 

**Tag filter format:**

Each input feature (node or way) is passed from the tag filter which aims to keep or discard features, and keep or discurd specific 
tags for each retained feature. The filtered tags are also associated with appropriate zoom-range intervals i.e. a tag may appear 
only in tiles with zoom value higher than a specified threshold. The filter file contains a sequence of rules which are defined 
recursively:
```
[<condition>] {
  <commands>+
  [ <condition> {<commands> }]*
}
...
```
Conditions are C like conditional expressions. A rule is a conditional command. If a condition is satisfied the command block is executed. The command block may consist of 
other conditional commands which are evaluated recursively like a nested if-the-else. When a command block is finally reached 
the commands in it are executed and no further rule is checked in the parent block unless the "continue" command is given. 
An example of a simple filter is as follows:
```
[ is_way() ] { 
    {   attach layer ; 
        [ area == 'yes' ] { write [z12] area ; continue ; }
        [ access in ( 'destination', 'private' ) ] { attach access ; continue ; }
        [ bridge == 'yes' ] { attach bridge ; continue ; }
        [ natural == 'wood' || landuse == 'forest' ] { attach leaf_type ; }  
        continue ; 
    }
    [ highway ] {
        attach tracktype, tunnel, oneway, surface, trail_visibility, sas_scale ; 
        [ highway in ('bridleway', 'bus_guideway', 'byway', 'construction', 'cycleway', 'footway', 'living_street', 'unclassified') ] { write [z13] highway ; }
        [ highway in ('path', 'pedestrian', 'services', 'service') ] { write [z14] highway ; }
        [ highway in ('motorway', 'motorway_link', 'trunk', 'trunk_link') ] { write [z6] highway ; }
        [ highway in ('primary', 'primary_link') ] { write [z8] highway ; }
        [ highway == 'unclassified' ] { write [z13] highway ; }
        [ highway == 'track' ] { write [z13] highway, track_type ; }
        [ highway in ('raceway', 'residential', 'road', 'secondary', 'secondary_link', 'tertiary', 'tertiary_link') ] { write [z12] highway ; }
        [ highway == 'steps' ] { write [z16] highway ; }
    }
```
Consider in the example above a way with "highway=secondary" tag given. The first rule "is_way()" matches and the command block
is visited.
The first command is a rule with no condition which means that it is always executed. The first command in the subrule is "attach layer".
The second command is a sub-sub-rule and so on. In our case nothing else matches and we reach "continue" command that tells us 
that we have to continue checking rules in the parent block. Then the "highway" (highway tag exists) rule is executed and the 
first command "attach" plus the second sub-rule from the end "write [z12] highway" are finally matched. Since there is no continue, 
no other rule is checked and the filter exits. The list of commands visited are 
```
attach layer ;
attach tracktype, tunnel, oneway, surface, trail_visibility, sas_scale ; 
write [z12] highway
```
The "attach" command works only for tags that exist and appends them to the list of the written tags only for the zoom intervals specified
in "write" commands. So for example if we had "surface=asphalt" input tag then the final tags would be "highway=secondary" and "surface=asphalt" and
they would be valid after zoom level 12. If we wanted "surface" tag to appear only after z15 then we should have two "write commands:
```
write [z12-z14] highway
write [z15-] highway, surface
```
List of commands:
```
add <tag> = <expression> ; // adds the tag to the current feature if not already exists
set <tag> = <expression> ; // inserts or update a tag in the current feature
delete <tag> ;             // delete tag from the current feature
write [<zoom_range>] <tag>[=<expression>], <tag>[=<expression>] ... ; // output tags in the given interval 
write all [<zoom_range>] ; // write all input tags
attach <tag_list> ;        // append these tags to each write command
exclude [<zoom_range>] <tag_list> ; // exclude these tags from the output
continue ;                 // process the next rule in the parent block
```
Well known relation types such as multi-polygons, boundaries and routes are converted into ways by stiching together their member ways. These should be matched using is_relation() function.

It is also possible to define custom functions in LUA by prepending the script in the config file.
```
<?
...lua code...
?>
... rules ...
```
All functions declared in the block have access to a global table "osm" that contains:
- a "tags" field, table containing all key/value pairs of the current feature.
- a "relations" field, an array of all relations that this feature is part of. Each relation is a table with associated key/value pairs. For example to test if a way is part of a hiking route one may write
```
function is_hk_route()
  for i = 1,#(osm.relations) do
    local rel = osm.relations[i] ;
    if rel.type == "route" and rel.route == "hiking" then return true end
  end 
  return false ;
end
```
- "add_tags" function taking as input a table of key/value pairs to add to each feature.
- "set_tags" function taking as input a table of key/value pairs to modify in each feature.
- "delete_tags" function taking as input a list (table) of keys to delete from each feature.
- "attach_tags" function taking as inpout a list (table) of keys and with same effect as the attach command.
- "write" function taking 3 to 4 arguments, zmin, zmax, key and optional value with same effect as the write command.
- "is_way", "is_node", "is_relation" functions that return true if the processed feature is of associated type.
