#include <assert.h>
#include <iostream>

#include "canvas.h"
#include "settings.h"

#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/BoundingBox>
#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/Plane>
#include <osg/BlendFunc>

Canvas::Canvas(osg::MatrixTransform *transform, const std::string &name):
    _switch(new osg::Switch),
    _transform(transform),
    _switchFrame(new osg::Switch),
    _transformData(new osg::MatrixTransform),
    _geodeFrame(new osg::Geode),
    _geodeAxis(new osg::Geode),
    _frame(new osg::Geometry),
    _pickable(new osg::Geometry),
    _axis(new osg::Geometry),
    _geodeData(new osg::Geode),

    _center(osg::Vec3f(0.0f, 0.0f, 0.0f)),
    _normal(osg::Vec3f(0.0f, -1.0f, 0.0f)),
    _plane(osg::Plane(_normal, _center)),
    _x(osg::Vec3f(1.f,0.f,0.f)),
    _y(osg::Vec3f(0.f,1.f,0.f)),
    _color(dureu::CANVAS_CLR_REST)

{
    this->setName(name);
    this->setColor(_color);

    osg::StateSet* stateset = new osg::StateSet;
    osg::LineWidth* linewidth = new osg::LineWidth();
    linewidth->setWidth(1.5);
    osg::BlendFunc* blendfunc = new osg::BlendFunc();
    //blendfunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ANTIALIAS);
    stateset->setAttributeAndModes(linewidth,osg::StateAttribute::ON);
    stateset->setAttributeAndModes(blendfunc, osg::StateAttribute::ON);
    stateset->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    this->setStateSet(stateset);

    this->addChild(_switch.get());
    _switch->addChild(_transform.get(), true);
    _transform->addChild(_switchFrame.get());
    _transform->addChild(_transformData.get());

    _transformData->addChild(_geodeData.get());
    // _geodeData is  empty, it is for user input: strokes

    _switchFrame->addChild(_geodeFrame.get(), true); // child #0
    _switchFrame->addChild(_geodeAxis.get(), true); // child #1

    _geodeFrame->addDrawable(_frame.get());
    _geodeFrame->addDrawable(_pickable.get());

    _geodeAxis->addDrawable(_axis.get());

     osg::Vec3Array* frameVertices = new osg::Vec3Array(4);
    (*frameVertices)[0] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*frameVertices)[1] = osg::Vec3f(-dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*frameVertices)[2] = osg::Vec3f(-dureu::CANVAS_MINW, -dureu::CANVAS_MINH, 0.0f);
    (*frameVertices)[3] = osg::Vec3f(dureu::CANVAS_MINW, -dureu::CANVAS_MINH, 0.0f);

     osg::Vec3Array* pickableVertices = new osg::Vec3Array(4);
    (*pickableVertices)[0] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH, 0.0f);
    (*pickableVertices)[1] = osg::Vec3f(dureu::CANVAS_MINW-dureu::CANVAS_CORNER, dureu::CANVAS_MINH, 0.0f);
    (*pickableVertices)[2] = osg::Vec3f(dureu::CANVAS_MINW-dureu::CANVAS_CORNER, dureu::CANVAS_MINH-dureu::CANVAS_CORNER, 0.0f);
    (*pickableVertices)[3] = osg::Vec3f(dureu::CANVAS_MINW, dureu::CANVAS_MINH-dureu::CANVAS_CORNER, 0.0f);

     osg::Vec3Array* axisVertices = new osg::Vec3Array(4);
     (*axisVertices)[0] = osg::Vec3f(0.f,0.f,0.f);
     (*axisVertices)[1] = osg::Vec3f(dureu::CANVAS_AXIS, 0.f,0.f);
     (*axisVertices)[2] = osg::Vec3f(0.f,0.f,0.f);
     (*axisVertices)[3] = osg::Vec3f(0.f,dureu::CANVAS_AXIS, 0.f);

     osg::Vec4Array* colorAxis = new osg::Vec4Array(4);
     (*colorAxis)[0] = solarized::base2;
     (*colorAxis)[1] = solarized::base2;
     (*colorAxis)[2] = solarized::base2;
     (*colorAxis)[3] = solarized::base2;

    _frame->setVertexArray(frameVertices);
    _frame->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_LOOP,0,4));

    _pickable->setVertexArray(pickableVertices);
    _pickable->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    _axis->setVertexArray(axisVertices);
    _axis->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,4));
    _axis->setColorArray(colorAxis, osg::Array::BIND_PER_VERTEX);
}

osg::Vec4f Canvas::getColor() const{
    return _color;
}

void Canvas::setVisibility(bool vis)
{
    _switch->setChildValue(_switch->getChild(0), vis);
}

bool Canvas::getVisibility() const{
    return _switch->getChildValue(_switch->getChild(0));
}

void Canvas::setTransform(osg::MatrixTransform *transform){
    _transform = transform;
}

osg::MatrixTransform *Canvas::getTransform() const
{
    return _transform.get();
}

void Canvas::setCanvasName(const std::string &name)
{
    this->setName(name);
    this->setSwitchName(name);
    this->setTransformName(name);
    this->setSwitchFrameName(name);
    this->setGeodeDataName(name);
}

void Canvas::setSwitchName(const std::string &parentName)
{
    _switch->setName(parentName + "Switch");
}

void Canvas::setTransformName(const std::string &parentName)
{
    _transform->setName(parentName + "Transform");
}

void Canvas::setSwitchFrameName(const std::string &parentName)
{
    _switchFrame->setName(parentName + "SwitchFrame");
}

void Canvas::setGeodeDataName(const std::string &parentName)
{
    _geodeData->setName(parentName + "GeodeData");
}

std::string Canvas::getSwitchName() const{
    return _switch->getName();
}

std::string Canvas::getTransformName() const{
    return _transform->getName();
}

std::string Canvas::getSwitchFrameName() const
{
    return _switchFrame->getName();
}

std::string Canvas::getGeodeDataName() const{
    return _geodeData->getName();
}

void Canvas::setColor(osg::Vec4 color){
    if (color == dureu::CANVAS_CLR_CURRENT) // hide the axis for "rest" canvases
        _switchFrame->setChildValue(_geodeAxis, true);
    else
        _switchFrame->setChildValue(_geodeAxis, false);

    _color = color;
    osg::Vec4Array* colors = new osg::Vec4Array(4);
    (*colors)[0] = _color;
    (*colors)[1] = _color;
    (*colors)[2] = _color;
    (*colors)[3] = _color;
    _frame->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    _pickable->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
}
