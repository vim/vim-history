" Vim syntax file
" Language:     VRML 1.0C
" Maintainer:   David Brown <dbrown@cgs.c4.gmeds.com>
" Last change:  1997 June 8

" keyword definitions
syn keyword VRMLNodes          AsciiText Cone Coordinate3 Cube Cylinder
syn keyword VRMLNodes          DirectionalLight Group IndexedFaceSet
syn keyword VRMLNodes          IndexedLineSet Info LevelOfDetail Material
syn keyword VRMLNodes          MaterialBinding MatrixTransform Normal
syn keyword VRMLNodes          NormalBinding OrthographicCamera
syn keyword VRMLNodes          PerspectiveCamera PointLight PointSet Rotation
syn keyword VRMLNodes          Scale Separator ShapeHints Sphere SpotLight
syn keyword VRMLNodes          Switch Texture2 Texture2Transform
syn keyword VRMLNodes          TextureCoordinate2 Transform
syn keyword VRMLNodes          TransformSeparator Translation UnknownNode
syn keyword VRMLNodes          WWWAnchor WWWInline

syn keyword VRMLFields         string spacing justification width parts
syn keyword VRMLFields         bottomRadius height point depth on intensity
syn keyword VRMLFields         color direction size family style coordIndex
syn keyword VRMLFields         materialIndex normalIndex textureCoordIndex
syn keyword VRMLFields         range ambientColor diffuseColor specularColor
syn keyword VRMLFields         emissiveColor shininess transparency value
syn keyword VRMLFields         matrix vector position orientation fields
syn keyword VRMLFields         focalDistance heightAngle location rotation
syn keyword VRMLFields         scaleFactor renderCulling vertexOrdering
syn keyword VRMLFields         shapeType faceType creaseAngle dropOffRate
syn keyword VRMLFields         cutOffAngle whichChild filename image wrapS
syn keyword VRMLFields         wrapT translation scaleOrientation center
syn keyword VRMLFields         name description map bboxSize bboxCenter
syn keyword VRMLFields         startIndex numPoints radius

syn keyword VRMLEnums          LEFT CENTER RIGHT SIDES BOTTOM ALL TOP SERIF
syn keyword VRMLEnums          SANS TYPEWRITER NONE BOLD ITALIC DEFAULT
syn keyword VRMLEnums          OVERALL PER_PART PER_PART_INDEXED PER_FACE
syn keyword VRMLEnums          PER_FACE_INDEXED PER_VERTEX PER_VERTEX_INDEXED
syn keyword VRMLEnums          ON OFF AUTO UNKNOWN_ORDERING CLOCKWISE
syn keyword VRMLEnums          COUNTERCLOCKWISE UNKNOWN_SHAPE_TYPE SOLID
syn keyword VRMLEnums          UNKOWN_FACE_TYPE CONVEX TRUE FALSE REPEAT
syn keyword VRMLEnums          CLAMP POINT
syn keyword VRMLEnums          SFBool SFColor SFEnum SFImage SFLong SFMatrix
syn keyword VRMLEnums          SFRotation SFString SFVec2f SFVec3f SFFloat
syn keyword VRMLEnums          MFColor MFLong MFVec2f MFVec3f


syn keyword VRMLInstances      DEF USE

" match definitions.
syn match   VRMLSpecial           contained "\\[0-9][0-9][0-9]\|\\."
syn region  VRMLString            start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=VRMLSpecial
syn match   VRMLCharacter         "'[^\\]'"
syn match   VRMLSpecialCharacter  "'\\.'"
syn match   VRMLNumber            "-\=\<[0-9]\+L\=\>\|0[xX][0-9a-fA-F]\+\>"
syn match   VRMLNumber            "0[xX][0-9a-fA-F]\+\>"
syn match   VRMLComment           "#.*$"

syn sync minlines=1 maxlines=1

if !exists("did_VRML_syntax_inits")
  let did_VRML_syntax_inits = 1
  " The default methods for highlighting.  Can be overridden later
  hi link VRMLCharacter  VRMLString
  hi link VRMLSpecialCharacter VRMLSpecial
  hi link VRMLNumber     VRMLString
  hi link VRMLString     String
  hi link VRMLSpecial    Special
  hi link VRMLComment	Comment
  hi link VRMLNodes      Statement
  hi link VRMLFields     Type
  hi link VRMLInstances  PreCondit
  hi link VRMLEnums      Identifier
endif

let b:current_syntax = "vrml"

" vim: ts=8
