" Vim syntax file
" This is a GENERATED FILE. Please always refer to source file at the URI below.
" Language: POV-Ray(tm) 3.1 Screen Description Language
" Maintainer: David Ne\v{c}as (Yeti) <yeti@physics.muni.cz>
" Last Change: 2001-07-25
" URI: http://physics.muni.cz/~yeti/download/pov.vim
" Required Vim Version: 6.0

" Setup
if version >= 600
  " Quit when a syntax file was already loaded
  if exists("b:current_syntax")
    finish
  endif
else
  " Croak when an old Vim is sourcing us.
  echo "Sorry, but this syntax file relies on Vim 6 features.  Either upgrade Vim or usea version of " . expand("<sfile>:t:r") . " syntax file appropriate for Vim " . version/100 . "." . version %100 . "."
  finish
endif

syn case match

" Top level stuff
syn keyword povCommands global_settings
syn keyword povObjects array atmosphere background bicubic_patch blob box camera cone cubic cylinder disc fog height_field julia_fractal lathe light_source mesh object plane poly polygon prism quadric quartic rainbow sky_sphere smooth_triangle sor sphere superellipsoid text torus triangle
syn keyword povCSG clipped_by difference intersection merge union
syn keyword povAppearance interior material media texture
syn keyword povGlobalSettings adc_bailout ambient_light assumed_gamma hf_gray_16 irid_wavelength max_intersections max_trace_level number_of_waves Number radiosity
syn keyword povTransform matrix rotate scale translate transform

" Descriptors
syn keyword povDescriptors finish normal pigment
syn keyword povDescriptors color colour rgb rgbt rgbf rgbft red green blue
syn keyword povDescriptors bump_map color_map colour_map image_map material_map pigment_map quick_color quick_colour normal_map texture_map
syn keyword povDescriptors ambient brilliance crand diffuse irid metallic phong phong_size reflection reflection_exponent roughness specular
syn keyword povDescriptors cylinder fisheye omnimax orthographic panoramic perspective ultra_wide_angle
syn keyword povDescriptors agate average brick boxed bozo bumps checker crackle cylindrical dents gradient granite hexagon leopard mandel marble onion planar quilted radial ripples spherical spiral1 spiral2 spotted waves wood wrinkles
syn keyword povDescriptors density_file nextgroup=povDensityType skipwhite skipnl
syn keyword povDescriptors area_light shadowless spotlight
syn keyword povDescriptors absorption confidence density emission intervals ratio samples scattering variance
syn keyword povDescriptors distance fog_alt fog_offset fog_type turb_depth
syn keyword povDescriptors brightness count distance_maximum error_bound gray_threshold low_error_factor minimum_reuse nearest_count recursion_limit

" Modifiers
syn keyword povModifiers caustics fade_distance fade_power ior
syn keyword povModifiers bounded_by hierarchy hollow no_shadow open smooth sturm threshold water_level
syn keyword povModifiers hypercomplex max_iteration precision quaternion slice
syn keyword povModifiers bezier_spline conic_sweep cubic_spline linear_spline linear_sweep quadratic_spline
syn keyword povModifiers flatness type u_steps v_steps
syn keyword povModifiers adaptive falloff jitter looks_like media_attenuation media_interaction point_at radius tightness
syn keyword povModifiers angle aperture blur_samples confidence direction focal_point location look_at right sky up variance
syn keyword povModifiers all bump_size filter interpolate map_type once slope_map transmit use_color use_colour use_index
syn keyword povModifiers black_hole agate_turb brick_size control0 control1 cubic_wave density_map flip frequency interpolate inverse lambda mortar octaves offset omega phase poly_wave ramp_wave repeat scallop_wave sine_wave strength triangle_wave thickness turbulence type warp
syn keyword povModifiers eccentricity extinction
syn keyword povModifiers arc_angle falloff_angle width

" Words not marked `reserved' in documentation, but...
syn keyword povHFType gif iff pgm png pot ppm sys tga contained
syn keyword povFontType ttf contained
syn keyword povDensityType df3 contained
syn cluster povPRIVATE add=povDensityType

" Math functions on floats, vectors and strings
syn keyword povFunctions abs acos asc asin atan2 ceil cos defined degrees dimensions dimension_size div exp file_exists floor int log max min mod pow radians rand seed sin sqrt strcmp strlen tan val vdot vlength
syn keyword povFunctions vaxis_rotate vcross vnormalize vrotate
syn keyword povFunctions chr concat substr str strupr strlwr
syn keyword povJuliaFunctions acosh asinh atan cosh cube pwr reciprocal sinh sqr tanh

" Specialities
syn keyword povConsts clock clock_delta false no off on pi t true u v version x y yes z
syn match povDotItem "\.\@<=\(blue\|green\|filter\|red\|transmit\|t\|u\|v\|x\|y\|z\)\>" display

" Comments
syn region povComment start="/\*" end="\*/" contains=povTodo,povComment
syn match povComment "//.*" contains=povTodo
syn match povCommentError "\*/"
syn sync ccomment povComment
syn sync minlines=50
syn keyword povTodo TODO FIXME XXX NOT contained
syn cluster povPRIVATE add=povTodo

" Language directives
syn match povConditionalDir "#\s*\(else\|end\|if\|ifdef\|ifndef\|switch\|while\)\>"
syn match povLabelDir "#\s*\(break\|case\|default\|range\)\>"
syn match povDeclareDir "#\s*\(declare\|default\|local\|macro\|undef\|version\)\>"
syn match povIncludeDir "#\s*include\>"
syn match povFileDir "#\s*\(fclose\|fopen\|read\|write\)\>"
syn match povMessageDir "#\s*\(debug\|error\|render\|statistics\|warning\)\>"
syn region povFileOpen start="#\s*fopen\>" skip=+"[^"]*"+ matchgroup=povOpenType end="\<\(read\|write\|append\)\>" contains=ALLBUT,PovParenError,PovBraceError,@PovPRIVATE transparent keepend

" Literal strings
syn match povSpecialChar "\\\d\d\d\|\\." contained
syn region povString start=+"+ skip=+\\\\\|\\"+ end=+"+ contains=povSpecialChar oneline
syn cluster povPRIVATE add=povSpecialChar

" Catch errors caused by wrong parenthesization
syn region povParen start='(' end=')' contains=ALLBUT,povParenError,@povPRIVATE transparent
syn match povParenError ")"
syn region povBrace start='{' end='}' contains=ALLBUT,povBraceError,@povPRIVATE transparent
syn match povBraceError "}"

" Numbers
syn match povNumber "\(^\|\W\)\@<=[+-]\=\(\d\+\)\=\.\=\d\+\([eE][+-]\=\d\+\)\="

" Define the default highlighting
hi def link povComment        Comment
hi def link povTodo           Todo
hi def link povNumber         Number
hi def link povString         String
hi def link povFileOpen       Constant
hi def link povConsts         Constant
hi def link povDotItem        Constant
hi def link povHFType         povSpecial
hi def link povDensityType    povSpecial
hi def link povFontType       povSpecial
hi def link povOpenType       povSpecial
hi def link povSpecialChar    povSpecial
hi def link povSpecial        Special
hi def link povConditionalDir PreProc
hi def link povLabelDir       PreProc
hi def link povDeclareDir     Define
hi def link povIncludeDir     Include
hi def link povFileDir        PreProc
hi def link povMessageDir     Debug
hi def link povAppearance     povDescriptors
hi def link povObjects        povDescriptors
hi def link povGlobalSettings povDescriptors
hi def link povDescriptors    Type
hi def link povJuliaFunctions PovFunctions
hi def link povModifiers      povFunctions
hi def link povFunctions      Function
hi def link povCommands       Operator
hi def link povTransform      Operator
hi def link povCSG            Operator
hi def link povParenError     povError
hi def link povBraceError     povError
hi def link povCommentError   povError
hi def link povError          Error

let b:current_syntax = "pov"
