" Vim syntax file
" Language:	POV-Ray(tm) 3.1 Screen Description Language
" Maintainer:	David Ne\v{c}as (Yeti) <yeti@physics.muni.cz>
" Last Change:	2000 Nov 04

" BETA VERSION
" Rewritten from scratch; has nothing to do with T. Scott Urban's pov.vim file.
" Bugfixes, suggestions and other invectives :-) are welcome.
" Note old keywords like alpha, component, composite, tiles, etc. are NOT highlighted

" Possible TODO: 
"   highlight more syntax errors
"   decide once for all what is descriptor and what is modifier or merge them together

" Remove any old syntax stuff hanging around
syn clear

syn case match
" `top level' stuff
syn keyword povCommands		global_settings
syn keyword povObjects		array atmosphere background bicubic_patch blob box camera cone cubic cylinder disc fog height_field julia_fractal lathe light_source mesh object plane poly polygon prism quadric quartic rainbow sky_sphere smooth_triangle sor sphere superellipsoid text torus triangle
syn keyword povCSG              clipped_by difference intersection merge union
syn keyword povAppearance	interior material media texture
syn keyword povGlobalSettings	adc_bailout ambient_light assumed_gamma hf_gray_16 irid_wavelength max_intersections max_trace_level number_of_waves Number radiosity
syn keyword povTransform	matrix rotate scale translate transform

" descriptors
syn keyword povDescriptors	finish normal pigment
" FIXME: filter and transmit are both descriptors and modifiers (i chose modifier)
"syn keyword povDescriptors	 color colour rgb rgbt rgbf rgbft red green blue filter transmit
syn keyword povDescriptors	color colour rgb rgbt rgbf rgbft red green blue
syn keyword povDescriptors	bump_map color_map colour_map image_map material_map pigment_map quick_color quick_colour texture_map
syn keyword povDescriptors	ambient brilliance crand diffuse irid metallic phong phong_size reflection reflection_exponent roughness specular
syn keyword povDescriptors	cylinder fisheye omnimax orthographic panoramic perspective ultra_wide_angle
syn keyword povDescriptors	agate average brick boxed bozo bumps checker crackle cylindrical density_file dents gradient granite hexagon leopard mandel marble onion planar quilted radial ripples spherical spiral1 spiral2 spotted waves wood wrinkles
" FIXME: cylinder is both object and descriptor (fortunately they're both linked to Type by default)
"syn keyword povDescriptors	 area_light cylinder shadowless spotlight
syn keyword povDescriptors	area_light shadowless spotlight
syn keyword povDescriptors	absorption confidence density emission intervals ratio samples scattering variance
syn keyword povDescriptors	distance fog_alt fog_offset fog_type turb_depth
syn keyword povDescriptors	brightness count distance_maximum error_bound gray_threshold low_error_factor minimum_reuse nearest_count recursion_limit

" modifiers (the border between descriptor and modifier is somewhat fuzzy)
syn keyword povModifiers	caustics fade_distance fade_power ior
syn keyword povModifiers	bounded_by hierarchy hollow no_shadow open smooth sturm threshold water_level
syn keyword povModifiers	hypercomplex max_iteration precision quaternion slice
syn keyword povModifiers	bezier_spline conic_sweep cubic_spline linear_spline linear_sweep quadratic_spline 
syn keyword povModifiers	flatness type u_steps v_steps
syn keyword povModifiers	adaptive falloff jitter looks_like media_attenuation media_interaction point_at radius tightness
syn keyword povModifiers	angle aperture blur_samples confidence direction focal_point location look_at right sky up variance
syn keyword povModifiers	all bump_size filter interpolate map_type normal_map once slope_map transmit use_color use_colour use_index
syn keyword povModifiers	black_hole agate_turb brick_size control0 control1 cubic_wave density_map flip frequency interpolate inverse lambda mortar octaves offset omega phase poly_wave ramp_wave repeat scallop_wave sine_wave strength triangle_wave thickness turbulence type warp
syn keyword povModifiers	eccentricity extinction
syn keyword povModifiers	arc_angle falloff_angle width

" these are not marked reserved in documentation, but...
syn keyword povHFType		gif iff pgm png pot ppm sys tga contained
syn keyword povFontType		ttf contained
syn keyword povDensityType	df3 contained

" math functions on floats, vectors and strings
syn keyword povFunctions	abs acos asc asin atan2 ceil cos defined degrees dimensions dimension_size div exp file_exists floor int log max min mod pow radians rand seed sin sqrt strcmp strlen tan val vdot vlength
syn keyword povFunctions	vaxis_rotate vcross vnormalize vrotate
syn keyword povFunctions	chr concat substr str strupr strlwr
" functions available in julia_fractal only
syn keyword povJuliaFunctions	acosh asinh atan cosh cube pwr reciprocal sinh sqr tanh

" specialities
syn keyword povConsts		clock clock_delta false no off on pi t true u v version x y yes z
syn match   povDotItem		"\.\(blue\|green\|filter\|red\|transmit\|t\|u\|v\|x\|y\|z\)\>"lc=1

" comments (can be nested)
syn region povComment		start="/\*" end="\*/" contains=povTodo,povComment
syn match  povComment		"//.*" contains=povTodo
syn match  povCommentError	"\*/"
syn sync ccomment povComment
syn keyword povTodo		TODO FIXME XXX contained

" language directives (highlight only the keyword)
syn match povConditionalDir	"#\s*\(else\|end\|if\|ifdef\|ifndef\|switch\|while\)\>"
syn match povLabelDir		"#\s*\(break\|case\|default\|range\)\>"
syn match povDeclareDir		"#\s*\(declare\|default\|local\|macro\|undef\|version\)\>"
syn match povIncludeDir		"#\s*include\>"
syn match povFileDir		"#\s*\(fclose\|fopen\|read\|write\)\>"
syn match povMessageDir		"#\s*\(debug\|error\|render\|statistics\|warning\)\>"
syn keyword povOpenType		read write append contained
syn region povFileOpen		start="#\s*fopen\>" skip=+"[^"]*"+ end="\<\(read\|write\|append\)\>" contains=ALLBUT,PovParenError,PovBraceError,PovTodo transparent keepend

" literal strings (must be one-liners)
syn match   povSpecialChar	"\\\d\d\d\|\\." contained
syn region  povString		start=+"+  skip=+\\\\\|\\"+  end=+"+  contains=povSpecialChar oneline

" catch errors caused by wrong parenthesis
syn region	povParen	start='(' end=')' contains=ALLBUT,povParenError,povSpecial,povTodo transparent
syn match	povParenError	")"
" idem for curly braces
syn region	povBrace	start='{' end='}' contains=ALLBUT,povBraceError,povSpecial,povTodo transparent
syn match	povBraceError	"}"

" numbers (POV-Ray knows only floats)
syn match  povNumber		"\W[+-]\=\(\d\+\)\=\.\=\d\+\([eE][+-]\=\d\+\)\="lc=1
" does anyone start lines with a number?
"syn match  povNumber		"^[+-]\=\(\d\+\)\=\.\=\d\+\([eE][+-]\=\d\+\)\="

" The default highlighting.
  " comments
hi def link povComment		Comment
  " todo
hi def link povTodo		Todo
  " constants
hi def link povNumber		Number
hi def link povString		String
hi def link povFileOpen		Constant
hi def link povConsts		Constant
hi def link povDotItem		Constant
  " specials
hi def link povHFType		povSpecial
hi def link povDensityType	povSpecial
hi def link povFontType		povSpecial
hi def link povOpenType		povSpecial
hi def link povSpecialChar	povSpecial
hi def link povSpecial		Special
  " preproc or like
hi def link povConditionalDir	PreProc
hi def link povLabelDir		PreProc
hi def link povDeclareDir	Define
hi def link povIncludeDir	Include
hi def link povFileDir		PreProc
hi def link povMessageDir	Debug
  " objects and descriptors
hi def link povAppearance	povDescriptors
hi def link povObjects		povDescriptors
hi def link povGlobalSettings	povDescriptors
hi def link povDescriptors	Type
  " functions
hi def link povJuliaFunctions	PovFunctions
hi def link povModifiers	povFunctions
hi def link povFunctions	Function
  " operators
hi def link povCommands		Operator
hi def link povTransform	Operator
hi def link povCSG		Operator
  " errors
hi def link povParenError	povError
hi def link povBraceError	povError
hi def link povCommentError	povError
hi def link povError		Error

let b:current_syntax = "pov"

" vim: ts=8
