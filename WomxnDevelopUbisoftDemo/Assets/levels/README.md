Correspondance 
=================

Correspondance between number in tile map and elements. Files can be found under `Assets\levels`. Datas is separated in two csv files. The map `-tiles` is for the plateforms and background, the map `-elem` is for the interactive elements and ennemies. 

Filename examples : `Level-TMPF-Niv-%02d-tiles.csv`  and  `Level-TMPF-Niv-%02d-elem.csv`

# Tiles map 
### Not walkable = plateform
0: Sol
1: Plateform 
2: Plateform
3: Plateform
4: Plateform
5: Plateform
6: Plateform
7: Plateform
8: Plateform
9: Plateform
### Terrain Elements 
10: Air (background) 
11: Water Top 
12: Water bottom (full) 
13: Lava Top 
14: Lava Bottom (full) 
15: Void (black) 
16: _Air_ (! Not handle !)
17: _Air_ (! Not handle !)
18: _Air_ (! Not handle !)
19: _Air_ (! Not handle !)
# Elements 
100: Spawn position
101: Checkpoint not activated
140: Water Top 
141: Water Top (mirorred)
142: Lava Top
143: Lava Top (mirrored) 

# Ennemies 
208: Mushroom (takes 1 tile) 
330: Cactus (takes 4 x 2 tiles) 