" Vim syntax file
" Language:	ANT build file (xml)
" Maintainer:	Johannes Zellner <johannes@zellner.org>
" Last Change:	Sat, 03 Nov 2001 19:52:12 +0100
" Filenames:	build.xml
" $Id$

" Quit when a syntax file was already loaded
if exists("b:current_syntax")
    finish
endif

runtime syntax/xml.vim

syn case ignore

if !exists('*AntSyntaxScript')
    fun AntSyntaxScript(tagname, synfilename)
	unlet b:current_syntax
	let s:include = expand("<sfile>:p:h").'/'.a:synfilename
	if filereadable(s:include)
	    exe 'syn include @ant'.a:tagname.' '.s:include
	else
	    exe 'syn include @ant'.a:tagname." $VIMRUNTIME/syntax/".a:synfilename
	endif

	exe 'syn region ant'.a:tagname
		    \." start=#<script[^>]\\{-}language\\s*=\\s*['\"]".a:tagname."['\"]\\(>\\|[^>]*[^/>]>\\)#"
		    \.' end=#</script>#'
		    \.' fold'
		    \.' contains=@ant'.a:tagname.',xmlCdataStart,xmlCdataEnd,xmlTag,xmlEndTag'
		    \.' keepend'
	exe 'syn cluster xmlRegionHook add=ant'.a:tagname
    endfun
endif

" TODO: add more script languages here ?
call AntSyntaxScript('javascript', 'javascript.vim')
call AntSyntaxScript('jpython', 'python.vim')


syn cluster xmlTagHook add=antElement

syn keyword antElement display project target fileset include patternset exclude excludesfile includesfile
syn keyword antElement display filterset filter filtersfile description filelist path pathelement mapper
syn keyword antElement display classpath tar tarfileset fail uptodate srcfiles jpcoverage arg socket jvmarg
syn keyword antElement display triggers method filters dependset targetfileset srcfilelist srcfileset
syn keyword antElement display targetfilelist vsscheckin java sysproperty execon env targetfile srcfile echo
syn keyword antElement display native2ascii jjtree chmod javadoc2 bottom source doctitle header
syn keyword antElement display excludepackage bootclasspath doclet param sourcepath link footer package group
syn keyword antElement display title deltree cvs ccmcreatetask jlink addfiles mergefiles tstamp format
syn keyword antElement display pathconvert map unjar patch sound success cccheckout mail p4reopen gzip
syn keyword antElement display p4label jpcovreport coveragepath copy jar manifest attribute section metainf
syn keyword antElement display zipfileset unzip rename mmetrics propertyfile entry copyfile ccmcheckintask
syn keyword antElement display p4submit gunzip antcall taskdef mkdir sql transaction replace replacefilter
syn keyword antElement display replacetoken replacevalue ear archives ant property vsshistory style p4sync
syn keyword antElement display fixcrlf sequential vssget genkey dname p4edit zip condition equals and not os
syn keyword antElement display available filepath or antstructure pvcs pvcsproject javah class p4counter
syn keyword antElement display javac src extdirs test testlet p4revert ccmreconfigure xmlvalidate cab typedef
syn keyword antElement display mparse ccuncheckout jpcovmerge ejbjar dtd weblogictoplink wlclasspath iplanet
syn keyword antElement display jboss weblogic borland support rpm p4have vsslabel cvspass wljspc move
syn keyword antElement display junitreport report p4change csc signjar javacc cccheckin iplanet-ejbc unwar
syn keyword antElement display ccmcheckin ilasm touch blgenclient get vsscheckout javadoc antlr record untar
syn keyword antElement display delete copydir war webinf classes lib ccupdate depend ccmcheckout parallel
syn keyword antElement display renameext exec apply maudit searchpath rmic sleep script

hi def link antElement Statement

let b:current_syntax = "ant"

" vim: ts=8
