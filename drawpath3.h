/*****
 * drawpath3.h
 *
 * Stores a path3 that has been added to a picture.
 *****/

#ifndef DRAWPATH3_H
#define DRAWPATH3_H

#include "drawelement.h"
#include "path3.h"
#include "beziercurve.h"

namespace camp {

class drawPath3 : public drawElement {
protected:
  const path3 g;
  triple center;
  bool straight;
  prc::RGBAColour color;
  bool invisible;
  Interaction interaction;
  triple Min,Max;
public:
#ifdef HAVE_GL
  static BezierCurve R;
#endif  
  drawPath3(path3 g, triple center, const pen& p, Interaction interaction,
            const string& key="") :
    drawElement(key), g(g), center(center), straight(g.piecewisestraight()),
    color(rgba(p)), invisible(p.invisible()), interaction(interaction),
    Min(g.min()), Max(g.max()) {}
    
  drawPath3(const double* t, const drawPath3 *s) :
    drawElement(s->KEY), g(camp::transformed(t,s->g)), straight(s->straight),
    color(s->color), invisible(s->invisible), interaction(s->interaction),
    Min(g.min()), Max(g.max()) {
    center=t*s->center;
  }
  
  virtual ~drawPath3() {}

  bool is3D() {return true;}
  
  void bounds(const double* t, bbox3& B) {
    if(t != NULL) {
      const path3 tg(camp::transformed(t,g));
      B.add(tg.min());
      B.add(tg.max());
    } else {
      B.add(Min);
      B.add(Max);
    }
  }
  
  void ratio(const double* t, pair &b, double (*m)(double, double), double,
             bool &first) {
    pair z;
    if(t != NULL) {
      const path3 tg(camp::transformed(t,g));
      z=tg.ratio(m);
    } else z=g.ratio(m);

    if(first) {
      b=z;
      first=false;
    } else b=pair(m(b.getx(),z.getx()),m(b.gety(),z.gety()));
  }
  
  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  void render(double, const triple&, const triple&, double,
              bool transparent);

  drawElement *transformed(const double* t);
};

class drawNurbsPath3 : public drawElement {
protected:
  size_t degree;
  size_t n;
  triple *controls;
  double *weights;
  double *knots;
  prc::RGBAColour color;
  bool invisible;
  triple Min,Max;
  
#ifdef HAVE_GL
  GLfloat *Controls;
  GLfloat *Knots;
#endif  
  
public:
  drawNurbsPath3(const vm::array& g, const vm::array* knot,
                 const vm::array* weight, const pen& p, const string& key="") :
    drawElement(key), color(rgba(p)), invisible(p.invisible()) {
    size_t weightsize=checkArray(weight);
    
    string wrongsize="Inconsistent NURBS data";
    n=checkArray(&g);
    
    if(n == 0 || (weightsize != 0 && weightsize != n))
      reportError(wrongsize);
    
    controls=new(UseGC) triple[n];
    
    size_t k=0;
    for(size_t i=0; i < n; ++i)
      controls[k++]=vm::read<triple>(g,i);
      
    if(weightsize > 0) {
      size_t k=0;
      weights=new(UseGC) double[n];
      for(size_t i=0; i < n; ++i)
        weights[k++]=vm::read<double>(weight,i);
    } else weights=NULL;
      
    size_t nknots=checkArray(knot);
    
    if(nknots <= n+1 || nknots > 2*n)
      reportError(wrongsize);

    degree=nknots-n-1;
    
    run::copyArrayC(knots,knot,0,NoGC);
    
#ifdef HAVE_GL
    Controls=NULL;
#endif  
  }
  
  drawNurbsPath3(const double* t, const drawNurbsPath3 *s) :
    drawElement(s->KEY), degree(s->degree), n(s->n), weights(s->weights),
    knots(s->knots), color(s->color), invisible(s->invisible) {
    controls=new(UseGC) triple[n];
    for(unsigned int i=0; i < n; ++i)
      controls[i]=t*s->controls[i];
    
#ifdef HAVE_GL
    Controls=NULL;
#endif    
  }
  
  bool is3D() {return true;}
  
  void bounds(const double* t, bbox3& b);
  
  virtual ~drawNurbsPath3() {}

  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  void displacement();
  void ratio(const double* t, pair &b, double (*m)(double, double), double fuzz,
             bool &first);
    
  void render(double size2, const triple& Min, const triple& Max,
              double perspective, bool transparent);
    
  drawElement *transformed(const double* t);
};

// Draw a pixel.
class drawPixel : public drawElement {
  triple v;
  pen p;
  prc::RGBAColour color;
  double width;
  bool invisible;
  triple Min,Max;
public:
#ifdef HAVE_GL
  static Pixel R;
#endif  
  drawPixel(const triple& v, const pen& p, double width, const string& key="")
    : drawElement(key), v(v), p(p), color(rgba(p)), width(width),
      invisible(p.invisible()) {}

  void bounds(const double* t, bbox3& B) {
    Min=Max=(t != NULL) ? t*v : v;
    B.add(Min);
  }
  
  void ratio(const double* t, pair &b, double (*m)(double, double), double,
             bool &first) {
    triple V=(t != NULL) ? t*v : v;
    pair z=pair(xratio(V),yratio(V));
              
    if(first) {
      b=z;
      first=false;
    } else b=pair(m(b.getx(),z.getx()),m(b.gety(),z.gety()));
  }
  
  void render(double size2, const triple& b, const triple& B,
              double perspective, bool transparent);
  
  bool write(prcfile *out, unsigned int *, double, groupsmap&);
  
  drawElement *transformed(const double* t);
};

}

#endif
