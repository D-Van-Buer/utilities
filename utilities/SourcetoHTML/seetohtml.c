/* seetops - make a HRML file that displays highlighted Interlisp source files
   using the ^F font tags to render keywords and comments appropriately,
   also renders _ as left arrow and ^ as up arrow

   Written by Darrel J. Van Buer, PhD 2022, released to public domain

   seetohtml [infile] [outfile]
Arguments omitted or given as - uses stdin/stdout
*/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

static int tton = 0, bon = 0, bigon = 0, ion = 0, smallon = 0;

/* output: header to set up array of fonts and heights.
   Line of input file split into different fonts and output something like
   x y moveto Fx setfont (some text)show Fy setfont (more text)show ...
   I think x position will be fixed at left margin, y decremented from previous line
   by tallest font in this line.  When x gets to bottom margin, output showpage
*/
void renderline(FILE *out, char* inputline) {
  /* one line of file into html with font annotations */
  int i, initWhite = 1;
  char nexF;
  
  if(inputline[strlen(inputline)-1] == '\n' || (inputline[strlen(inputline)-1] == '\r'))
    inputline[strlen(inputline)-1] = '\0';
  /* still need to fix leading white space as HTML renders naked leading white as no space */
  for(i = 0; inputline[i]; i++) {
    if(inputline[i] != ' ') initWhite = 0;
    switch(inputline[i]) {
    case ' ':
      /* leading white space is hard to manage. Possible ways:
	 each line as a <p sytle="margin-left:NNpx"> text of line </p> - but pixels/mm/inch don't fit well with unknown window size
	 same problem with <spacer type=horizontal size>
	 <pre> ... </pre> does white but all monospaced.  (Don't know if <b> etc honored) */
      if(initWhite) fprintf(out, "&nbsp");  // supress HTML ignoring leading white space
      else fprintf(out, " ");
      break;
    case 6: /* font tag */
      nexF = inputline[++i];
      switch(nexF) {
      case 1: // tt only
	if(bigon) {
	  fprintf(out, "</big>");
	  bigon = 0;
	}
	if(ion) {
	  fprintf(out, "</i>");
	  ion = 0;
	}
	if(smallon) {
	  fprintf(out, "</small>");
	  smallon = 0;
	}
	if(bon) {
	  fprintf(out, "</b>");
	  bon = 0;
	}
	if(! tton) {
	  fprintf(out, "<tt>");
	  tton = 1;
	}
	break;
      case 2: // bold only
	if(bigon) {
	  fprintf(out, "</big>");
	  bigon = 0;
	}
	if(ion) {
	  fprintf(out, "</i>");
	  ion = 0;
	}
	if(smallon) {
	  fprintf(out, "</small>");
	  smallon = 0;
	}
	if(tton) {
	  fprintf(out, "</tt>");
	  tton = 0;
	}
	if(!bon) {
	  fprintf(out, "<b>");
	  bon = 1;
	}
	break;
      case 3: // tt small
	if(bigon) {
	  fprintf(out, "</big>");
	  bigon = 0;
	}
	if(ion) {
	  fprintf(out, "</i>");
	  ion = 0;
	}
	if(bon) {
	  fprintf(out, "</b>");
	  bon = 0;
	}
	if(! tton) {
	  fprintf(out, "<tt>");
	  tton = 1;
	}
	if(!smallon) {
	  fprintf(out, "<small>");
	  smallon = 1;
	}
	break;
      case 4: // big bold
	if(bigon && (ion || smallon || tton)) { // nested so end to unnest
	  fprintf(out, "</big>");
	  bigon = 0;
	}
	if(ion) {
	  fprintf(out, "</i>");
	  ion = 0;
	}
	if(smallon) {
	  fprintf(out, "</small>");
	  smallon = 0;
	}
	if(tton) {
	  fprintf(out, "</tt>");
	  tton = 0;
	}
	if(!bon) {
	  fprintf(out, "<b>");
	  bon = 1;
	}
	if(! bigon) {
	  fprintf(out, "<big>");
	  bigon = 1;
	}
	break;
      case 5: // plain (rare in source code?)
	if(bigon) {
	  fprintf(out, "</big>");
	  bigon = 0;
	}
	if(ion) {
	  fprintf(out, "</i>");
	  ion = 0;
	}
	if(smallon) {
	  fprintf(out, "</small>");
	  smallon = 0;
	}
	if(bon) {
	  fprintf(out, "</b>");
	  bon = 0;
	}
	if(tton) {
	  fprintf(out, "</tt>");
	  tton = 0;
	}
	break;
      case 6: // small italic for coments
	if(bigon) {
	  fprintf(out, "</big>");
	  bigon = 0;
	}
	if(ion && (bon || tton)) {
	  fprintf(out, "</i>");
	  ion = 0;
	}
	if(smallon &&(bon || tton)) {
	  fprintf(out, "</small>");
	  smallon = 0;
	}
	if(bon) {
	  fprintf(out, "</b>");
	  bon = 0;
	}
	if(tton) {
	  fprintf(out, "</tt>");
	  tton = 0;
	}
	if(!smallon) {
	  fprintf(out, "<small>");
	  smallon = 1;
	}
	if(! ion) {
	  fprintf(out, "<i>");
	  ion = 1;
	}
	break;
      case 7: // bold only
	if(bigon) {
	  fprintf(out, "</big>");
	  bigon = 0;
	}
	if(ion) {
	  fprintf(out, "</i>");
	  ion = 0;
	}
	if(smallon) {
	  fprintf(out, "</small>");
	  smallon = 0;
	}
	if(bon && tton) {
	  fprintf(out, "</b>");
	  bon = 0;
	}
	if(tton) {
	  fprintf(out, "</tt>");
	  tton = 0;
	}
	if(! bon) {
	  fprintf(out, "<bb>");
	  bon = 1;
	}
	break;
      default:
	fprintf(stderr, "unknown font code %d\n", nexF);
      }
      break;
      

      // all the characters that can't be naked in HTML
    case '<':
      fprintf(out, "&lt;");
      break;
    case '>':
      fprintf(out, "&gt;");
      break;
    case '&':
      fprintf(out, "&amp;");
      break;
      // note that single and double quotes only need escaping in attributes
    case '_':  // left arrow assignment
      fprintf(out, "&larr;");
      break;
    case '^':  // up arrow
      fprintf(out, "&uarr;");
      break;
   case 0: // end of line
     fprintf(out, "<br>\n");
      break; 
    default:
      fprintf(out, "%c", inputline[i]);
    }
  }
}


int main(int argc, char **argv) {
  char inputline[500], *result;
  FILE *in, *out;
  
  if(argc > 1) {
    if(!strcmp(argv[1], "-")) in = stdin;
    else {
      in = fopen(argv[1], "r");
      if(in == NULL) {
	fprintf(stderr, "Unable to open '%s': %s\n",
		argv[1], strerror(errno));
	exit(EXIT_FAILURE);
      }
    }
    if(argc > 2) {
      if(!strcmp(argv[2], "-")) out = stdout;
      else {
	out = fopen(argv[2], "w");
	if(out == NULL) {
	  fprintf(stderr, "Unable to open '%s': %s\n",
		  argv[2], strerror(errno));
	  exit(EXIT_FAILURE);
	}
      }
    }
    else out = stdout;
  }
  else {
    in = stdin;
    out = stdout;
  }

  /* notes:  font 1 - <tt>, font 2 <b>, font 3 <tt><small>, font 4 <b><big>, font 5 no tag Roman, font 6 <i>, font 7 <b> Roman
     Not sure that 3 5 or 7 used in source files */
  /* result should be something like
     <html><head> <title>  F4 filename F1 from line 1 or 2 of file </title></head>
     <body>
     <tt> much of text </tt> and insert extra tags for embelishments inside
     </body></html>
  */
  fprintf(out, "<html><head>\n");
  {
    char dfoline[500], filename[500], *begin, *endnm;
    int hasdfo = 0;
    /* special case, get filecreated info for title */
    result = fgets(inputline, sizeof(inputline), in);
    if(result == NULL) {
      fprintf(stderr, "NULL from fgets\n");
      exit(-1);
    }
    if(strstr(inputline, "DEFINE-FILE-INFO")) {
      hasdfo = 1;
      strcpy(dfoline, inputline);
    }
    else { // no define-file-info line on very old source file?
      if(strstr(inputline, "FILECREATED")) goto getTitle;
	begin = strstr(inputline, "\006\004");
	if(begin) {
	  endnm = strstr(begin+2, "\006\001");
	  if(endnm) {
	    strncpy(filename, begin+2, endnm-begin-2);
	    filename[endnm-begin-2] = 0;
	    fprintf(out, "<title>");
	    renderline(out, filename);
	    fprintf(out, "</title>\n");
	  }
	}
      	fprintf(out, "</head>\n");
	// either way, need to print line to file
	tton = 1;
	fprintf(out, "<tt>");
	renderline(out, inputline);
	fprintf(out, "<br>\n");
	goto nextl;
      }

    result = fgets(inputline, sizeof(inputline), in);
    if(result == NULL) {
      fprintf(stderr, "NULL from fgets\n");
      }
 getTitle:
    if(strstr(inputline, "FILECREATED")) {
	begin = strstr(inputline, "\006\004");
	if(begin) {
	  endnm = strstr(begin+2, "\006\001");
	  if(endnm) {
	    strncpy(filename, begin+2, endnm-begin-2);
	    filename[endnm-begin-2] = 0;
	    fprintf(out, "<title>");
	    renderline(out, filename);
	    fprintf(out, "</title>\n");
	  }
	}
      }
      fprintf(out, "</head>\n");
      tton = 1;
      fprintf(out, "<tt>");
      if(hasdfo) {
	renderline(out, dfoline);
	fprintf(out, "<br>\n");
      }
      renderline(out, inputline);
      fprintf(out, "<br>\n");
  }
 nextl:
  result = fgets(inputline, sizeof(inputline), in);
  if(result == NULL) {
    fprintf(stderr, "NULL from fgets\n");
  }
  if(feof(in)) {
    fprintf(out, "</html>\n");
    return 0;
  }
  renderline(out, inputline);
  fprintf(out, "<br>\n");
  goto nextl;
 }
