" Vim syntax file
" This is a GENERATED FILE. Please always refer to source file at the URI below.
" Language: PoV-Ray(tm) 3.1 configuration/initialization files
" Maintainer: David Ne\v{c}as (Yeti) <yeti@physics.muni.cz>
" Last Change: 2001-08-02
" URI: http://physics.muni.cz/~yeti/download/povini.vim
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

syn case ignore

" Syntax
syn match poviniInclude "^\s*[^[+-;]\S*\s*$" contains=poviniSection
syn match poviniLabel "^.\{-1,}\ze=" transparent contains=poviniKeyword nextgroup=poviniBool,poviniNumber
syn keyword poviniBool On Off True False Yes No
syn match poviniNumber "\<\d*\.\=\d\+\>"
syn keyword poviniKeyword Cyclic_Animation Field_Render Odd_Field Test_Abort Continue_Trace Create_Ini Display Pause_When_Done Verbose Draw_Vistas Output_to_File Buffer_Output Histogram_Type Output_Alpha Create_Ini Debug_Console Fatal_Console Render_Console Statistic_Console Warning_Console All_Console Radiosity Bounding Light_Buffer Vista_Buffer Remove_Bounds Split_Unions Antialias Jitter
syn keyword poviniKeyword Initial_Frame Final_Frame Initial_Clock Final_Clock Subset_Start_Frame Subset_Start_Frame Subset_End_Frame Subset_End_Frame Height Width Start_Column Start_Row End_Column End_Row Test_Abort_Count Display_Gamma Preview_Start_Size Preview_End_Size Bits_Per_Color Buffer_Size Histogram_Grid_Size Version Quality Bounding_Threshold Sampling_Method Antialias_Threshold Jitter_Amount Antialias_Depth
syn keyword poviniKeyword Create_Ini Debug_File Fatal_File Render_File Statistic_File Warning_File All_File
syn keyword poviniKeyword Output_File_Name Histogram_Name Input_File_Name Library_Path
syn keyword poviniKeyword Video_Mode Palette Histogram_Type Output_File_Type Pre_Scene_Return Pre_Frame_Return Post_Scene_Return Post_Frame_Return User_Abort_Return Fatal_Error_Return
syn match poviniShellOut "^\s*\(Pre_Scene_Command\|Pre_Frame_Command\|Post_Scene_Command\|Post_Frame_Command\|User_Abort_Command\|Fatal_Error_Command\)\>" nextgroup=poviniShellOutEq skipwhite
syn match poviniShellOutEq "=" nextgroup=poviniShellOutRHS skipwhite contained
syn match poviniShellOutRHS "[^;]\+" skipwhite contained contains=poviniShellOutSpecial
syn match poviniShellOutSpecial "%[osnkhw%]" contained
syn match poviniComment ";.*$"
syn match poviniOption "^\s*[+-]\S*"
syn match poviniIncludeLabel "^\s*Include_INI\s*=" nextgroup=poviniIncludedFile skipwhite
syn match poviniIncludedFile "[^;]\+" contains=poviniSection contained
syn region poviniSection start="\[" end="\]"

" Define the default highlighting
hi def link poviniSection Special
hi def link poviniComment Comment
hi def link poviniShellOut poviniKeyword
hi def link poviniIncludeLabel poviniKeyword
hi def link poviniKeyword Type
hi def link poviniShellOutSpecial Special
hi def link poviniIncludedFile poviniInclude
hi def link poviniInclude Include
hi def link poviniOption Keyword
hi def link poviniBool Constant
hi def link poviniNumber Number

let b:current_syntax = "povini"
