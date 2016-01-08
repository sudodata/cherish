#include "EventHandler.h"
#include <iostream>
#include <assert.h>
#include <osgViewer/View>
#include <osgUtil/LineSegmentIntersector>
#include <osgUtil/IntersectionVisitor>
#include <osg/Viewport>

EventHandler::EventHandler(RootScene *scene, dureu::MOUSE_MODE mode)
    : mMode(mode)
    , mScene(scene)
{
}

// handle() has to be re-defined, for more info, check
// OpenSceneGraph beginner's guide or
// OpenSceneGraph 3.0 Cookbook
// and search for custom event handler examples
bool EventHandler::handle(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    /*switch(ea.getEventType()){
    case osgGA::GUIEventAdapter::PEN_PRESSURE:
        outLogMsg("OSG pressure");
        break;
    case osgGA::GUIEventAdapter::PEN_PROXIMITY_ENTER:
        outLogMsg("OSG enter");
        break;
    case osgGA::GUIEventAdapter::PEN_PROXIMITY_LEAVE:
        outLogMsg("OSG leave");
        break;
    case osgGA::GUIEventAdapter::PUSH:
        outLogMsg("OSG push");
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        outLogMsg("OSG release");
        break;
    default:
        break;
    }*/

    /*switch (ea.getButtonMask()){
    case osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON:
        outLogMsg("OSG left button");
        break;
    case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON:
        outLogMsg("OSG mid button");
        break;
    case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON:
        outLogMsg("OSG right button");
        break;
    default:
        break;
    }*/

    /*switch (ea.getTabletPointerType()){
    case osgGA::GUIEventAdapter::PEN:
        outLogMsg("OSG pen");
        break;
    case osgGA::GUIEventAdapter::ERASER:
        outLogMsg("OSG eraser");
        break;
    }*/

    // if it's mouse navigation mode, don't process event
    // it will be processed by mouse navigator
    if (mMode == dureu::MOUSE_ROTATE || mMode == dureu::MOUSE_PAN ||
            mMode == dureu::MOUSE_ZOOM || mMode == dureu::MOUSE_FIXEDVIEW)
        return false;

    if (!mScene->getCanvasCurrent())
        return false;

    if (mMode == dureu::MOUSE_PICK || mMode == dureu::MOUSE_DELETE)
        doByLineIntersector(ea, aa);
    else if (mMode == dureu::MOUSE_EDIT_MOVE)
        doByHybrid(ea, aa);
    else
        doByRaytrace(ea, aa);
    return false;
}

void EventHandler::setMode(dureu::MOUSE_MODE mode)
{
    mMode = mode;
}

// for pick and erase when lineintersector is going to be used
// line intersector is already implemented within OSG
// so we only need to return the intersection results which is
// already sorted from closest to farthest
void EventHandler::doByLineIntersector(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    // proceed only if it is release of left mouse button
    if (ea.getEventType()!=osgGA::GUIEventAdapter::RELEASE ||
            ea.getButton()!=osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
        return;

    osgUtil::LineSegmentIntersector::Intersection* result = new osgUtil::LineSegmentIntersector::Intersection;
    bool intersected = this->getLineIntersections(ea,aa, *result);
    if (!intersected)
        return;

    switch (mMode) {
    case dureu::MOUSE_PICK:
        doPick(*result);
        break;
    case dureu::MOUSE_DELETE:
        doDelete(*result);
        break;
    default:
        break;
    }
}

void EventHandler::doByRaytrace(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            ))
        return;

    double u=0, v=0;
    osg::Vec3f XC = osg::Vec3f(0.f,0.f,0.f);

    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        std::cout << "doByRayTrace(): push button" << std::endl;
        switch(mMode){
        case dureu::MOUSE_SKETCH:
            if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
                return;
            doSketch(u, v, dureu::EVENT_PRESSED);
            break;
        case dureu::MOUSE_EDIT_OFFSET:
            if (!this->getRaytraceNormalProjection(ea,aa,XC))
                return;
            doEditOffset(XC, 0);
            break;
        case dureu::MOUSE_EDIT_ROTATE:
            doEditRotate(ea.getX(), ea.getY(), 0);
            break;
        case dureu::MOUSE_ERASE:
            if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
                return;
            doErase(u,v,0);
            break;
        default:
            break;
        }
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        std::cout << "doByRayTrace(): release button" << std::endl;
        switch(mMode){
        case dureu::MOUSE_SKETCH:
            if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
                return;
            doSketch(u, v, dureu::EVENT_RELEASED);
            break;
        case dureu::MOUSE_EDIT_OFFSET:
            if (!this->getRaytraceNormalProjection(ea,aa,XC))
                return;
            doEditOffset(XC, 2);
            break;
        case dureu::MOUSE_EDIT_ROTATE:
            doEditRotate(ea.getX(), ea.getY(), 2);
            break;
        case dureu::MOUSE_ERASE:
            if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
                return;
            doErase(u,v,2);
            break;
        default:
            break;
        }
        break;
    case osgGA::GUIEventAdapter::DRAG:
        switch(mMode){
        case dureu::MOUSE_SKETCH:
            if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
                return;
            doSketch(u, v, dureu::EVENT_DRAGGED);
            break;
        case dureu::MOUSE_EDIT_OFFSET:
            if (!this->getRaytraceNormalProjection(ea,aa,XC))
                return;
            doEditOffset(XC, 1);
            break;
        case dureu::MOUSE_EDIT_ROTATE:
            doEditRotate(ea.getX(), ea.getY(), 1);
            break;
        case dureu::MOUSE_ERASE:
            if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
                return;
            doErase(u,v,1);
            break;
        default:
            break;
        }
        break;
    default: // scrolling, doubleclick, move, keydown, keyup, resize
        // frame, pen_pressure, pen_..., ...
        break;
    }
}

// First it uses intersector to select a drawable, if there is
// no current drawable already (no drawable selected already);
// Then it uses operator to find intersection with canvas plane manually;
// For intersector, we can set a mask so that it only selects within current canvas,
// or we may switch the current canvas where the selected drawable belongs to;
void EventHandler::doByHybrid(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa)
{
    if (!( (ea.getEventType() == osgGA::GUIEventAdapter::PUSH && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::DRAG && ea.getButtonMask()== osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
           || (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE && ea.getButton()==osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
            ))
        return;

    osgUtil::LineSegmentIntersector::Intersection* result = new osgUtil::LineSegmentIntersector::Intersection;
    bool intersected = this->getLineIntersections(ea,aa, *result);
    if (!intersected)
        return;

    double u=0, v=0;
    if (!this->getRaytraceCanvasIntersection(ea,aa,u,v))
        return;

    switch (ea.getEventType()){
    case osgGA::GUIEventAdapter::PUSH:
        std::cout << "doByHybrid(): push button" << std::endl;
        outLogVec("u v", u, v, 0);
        doEditMove(*result, u, v, 0);
        break;
    case osgGA::GUIEventAdapter::RELEASE:
        std::cout << "doByHybrid(): release button" << std::endl;
        outLogVec("u v", u, v, 0);
        doEditMove(*result, u, v, 2);
        break;
    case osgGA::GUIEventAdapter::DRAG:
        doEditMove(*result, u, v, 1);
        break;
    default: // scrolling, doubleclick, move, keydown, keyup, resize
        // frame, pen_pressure, pen_..., ...
        break;
    }
}

void EventHandler::doPick(const osgUtil::LineSegmentIntersector::Intersection &result){
    Canvas* cnv = getCanvas(result);
    if (!cnv){
        std::cerr << "doPick(): could not dynamic_cast<Canvas*>" << std::endl;
        return;
    }
    std::cout << "doPick(): assumed canvas with name: " << cnv->getName() << std::endl;
    mScene->setCanvasCurrent(cnv);
}

// check nodepath to see how to go far enough so that to get canvas type
void EventHandler::doDelete(const osgUtil::LineSegmentIntersector::Intersection &result)
{
    Canvas* cnv = getCanvas(result);
    if (!cnv){
        std::cerr << "doDelete(): could not dynamic_cast<Canvas*>" << std::endl;
        return;
    }
    std::cout << "doDelete(): assumed canvas with name: " << cnv->getName() << std::endl;
    //bool success = mScene->deleteCanvas(cnv);
    //std::cout << "doDelete(): success is " << success << std::endl;
}

void EventHandler::doErase(double u, double v, int mouse)
{
    // Algorithm. For a center with 3d local [u,v,0],
    // check if there are any strokes whose points are from the center
    // within radius of ERASER_MIN;
    // If yes, mark those points and pass their coords into splitStroke
    // remove those points from each of the strokes, split strokes if needed
    // read more on point selection in OSG cookbook, Chapter 3 "Selection a point
    // of the model".
}

// see https://www.opengl.org/sdk/docs/man2/xhtml/gluUnProject.xml
// and https://www.mail-archive.com/osg-users@openscenegraph.net/msg16244.html
// for more details
// mouse is to indicate if the stroke is just created (0), in the process of drawing (1)
// or finished drawing (2)
void EventHandler::doSketch(double u, double v, dureu::EVENT event)
{
    mScene->addStroke(u, v, event);
    //mScene->getCanvasCurrent()->addStroke(u,v, event);
}

// performs offset of the current canvas along its normal
void EventHandler::doEditOffset(osg::Vec3f XC, int mouse)
{
    mScene->setTransformOffset(XC, mouse);
}

void EventHandler::doEditRotate(int x, int y, int mouse)
{
    mScene->setTransformRotate(osg::Vec3f(0,0,0), mouse);
}

// Pick photo
// Obtain canvas that contains that photo
// Make that canvas current
// Change photo coordinates to [x, y]
// Change photo colors when pushed (edit color) and released (no color)
void EventHandler::doEditMove(const osgUtil::LineSegmentIntersector::Intersection &result, double u, double v, int mouse)
{
    entity::Photo* photo = getPhoto(result);
    if (!photo){
        std::cerr << "doEditMove(): could not dynamic_cast<Photo*>" << std::endl;
        return;
    }
    Canvas* cnv = getCanvas(result);
    if (!cnv){
        std::cerr << "doEditMove(): could not dynamic_cast<Canvas*>" << std::endl;
        return;
    }
    mScene->setCanvasCurrent(cnv);
    mScene->getCanvasCurrent()->movePhoto(photo, u, v, mouse);
}

Canvas *EventHandler::getCanvas(const osgUtil::LineSegmentIntersector::Intersection &result){
    return dynamic_cast<Canvas*>(result.nodePath.at(mScene->getCanvasLevel()));
}

entity::Photo *EventHandler::getPhoto(const osgUtil::LineSegmentIntersector::Intersection &result)
{
    return dynamic_cast<entity::Photo*>(result.drawable.get());
}

bool EventHandler::getLineIntersections(const osgGA::GUIEventAdapter &ea,
                                        osgGA::GUIActionAdapter &aa,
                                        osgUtil::LineSegmentIntersector::Intersection &result)
{
    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        outErrMsg("getLineIntersections(): could not retrieve viewer");
        return false;
    }
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(
                osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());

    osgUtil::IntersectionVisitor iv(intersector);
    osg::Camera* cam = viewer->getCamera();
    if (!cam){
        std::cerr << "getLineIntersections(): could not read camera" << std::endl;
        return false;
    }
    cam->accept(iv);
    if (!intersector->containsIntersections()){
        outLogMsg("getLineIntersections(): no intersections found");
        return false;
    }
    result = *(intersector->getIntersections().begin());
    return true;
}

bool EventHandler::getRaytraceCanvasIntersection(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa,
                                           double &u, double &v)
{
    double x = ea.getX();
    double y = ea.getY();

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        std::cerr << "getRaytraceCanvasIntersection(): could not read viewer" << std::endl;
        return false;
    }

    osg::Camera* camera = viewer->getCamera();
    if (!camera){
        std::cout << "getRaytraceCanvasIntersection(): could not read camera" << std::endl;
        return false;
    }

    if (!camera->getViewport()){
        std::cerr << "getRaytraceIntersection(): could not read viewport" << std::endl;
        return false;
    }

    osg::Matrix VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    osg::Matrix invVPW;
    if (!invVPW.invert(VPW)){
        std::cerr << "getRaytraceIntersection(): could not invert View-projection-world matrix for ray casting" << std::endl;
        return false;
    }

    // Algorithm:
    // use ray-tracking techinique
    // calcualte near and far point in global 3D
    // intersect that segment with plane of canvas - 3D intersection point
    // extract local 3D coords so that to create a stroke (or apprent that point to a current stroke)
    osg::Vec3f nearPoint = osg::Vec3f(x, y, 0.f) * invVPW;
    osg::Vec3f farPoint = osg::Vec3f(x, y, 1.f) * invVPW;

    const osg::Plane plane = mScene->getCanvasCurrent()->getPlane();
    const osg::Vec3f center = mScene->getCanvasCurrent()->getCenter();

    assert(plane.valid());
    std::vector<osg::Vec3f> ray(2);
    ray[0] = nearPoint;
    ray[1] = farPoint;
    if (plane.intersect(ray)){ // 1 or -1: no intersection
        std::cerr << "getRaytraceIntersection(): no intersection with the ray." << std::endl;
        // finish the stroke if it was started
        // this should be replaced by a function finishAll()
        // which checks what are the current modes (sketch, photo move, etc) that are not finished
        // and finishes each which is current
        if (mScene->getCanvasCurrent()->getStrokeCurrent()){
            std::cout << "getRaytraceIntersection(): finishing the current stroke." << std::endl;
            //mScene->getCanvasCurrent()->finishStrokeCurrent();
            mScene->addStroke(0,0, dureu::EVENT_OFF);
        }
        return false;
    }
    osg::Vec3f dir = farPoint-nearPoint;
    if (! plane.dotProductNormal(dir)){ // denominator
        std::cerr << "getRaytraceIntersection(): projected line is parallel to the canvas plane" << std::endl;
        return false;
    }
    if (! plane.dotProductNormal(center-nearPoint)){
        std::cerr << "getRaytraceIntersection(): plane contains the line, so no single intersection can be defined" << std::endl;
        return false;
    }

    double len = plane.dotProductNormal(center-nearPoint) / plane.dotProductNormal(dir);
    osg::Vec3f P = dir * len + nearPoint;
    //outLogVec("getRaytraceIntersection(): intersect point global 3D", P.x(), P.y(), P.z());

    osg::Matrix M =  mScene->getCanvasCurrent()->getTransform()->getMatrix();
    osg::Matrix invM;
    if (!invM.invert(M)){
        std::cerr << "getRaytraceIntersection(): could not invert model matrix" << std::endl;
        return false;
    }
    osg::Vec3f p = P * invM;
    if (std::fabs(p.z())>dureu::EPSILON){
        std::cerr << "getRaytraceIntersection(): error while projecting point from global 3D to local 3D, z-coordinate is not zero" << std::endl;
        outLogVec("p", p.x(), p.y(), p.z());
        outLogVec("P", P.x(), P.y(), P.z());
        outLogVec("Normal", plane.getNormal().x(), plane.getNormal().y(), plane.getNormal().z());
        outLogVec("Center", center.x(), center.y(), center.z());
        return false;
    }

    u=p.x();
    v=p.y();
    return true;
}

bool EventHandler::getRaytraceNormalProjection(const osgGA::GUIEventAdapter &ea, osgGA::GUIActionAdapter &aa, osg::Vec3f& XC)
{
    double x = ea.getX();
    double y = ea.getY();

    osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
    if (!viewer){
        std::cerr << "getRaytraceNormalProjection(): could not read viewer" << std::endl;
        return false;
    }

    osg::Camera* camera = viewer->getCamera();
    if (!camera){
        std::cout << "getRaytraceNormalProjection(): could not read camera" << std::endl;
        return false;
    }

    if (!camera->getViewport()){
        std::cerr << "getRaytraceNormalProjection(): could not read viewport" << std::endl;
        return false;
    }

    osg::Matrix VPW = camera->getViewMatrix()
            * camera->getProjectionMatrix()
            * camera->getViewport()->computeWindowMatrix();
    osg::Matrix invVPW;
    if (!invVPW.invert(VPW)){
        std::cerr << "getRaytraceNormalProjection(): could not invert View-projection-world matrix for ray casting" << std::endl;
        return false;
    }

    // Algorithm:
    // Cast the ray into 3D space
    // Make sure the ray is not parallel to the normal
    // The new offset point will be located on the projected point
    // between the ray and canvas normal.
    // Ray and normal are skew lines in 3d space, so we only need
    // to extract the projection point of the ray into the normal.

    osg::Vec3f nearPoint = osg::Vec3f(x, y, 0.f) * invVPW;
    osg::Vec3f farPoint = osg::Vec3f(x, y, 1.f) * invVPW;
    osg::Vec3f C = mScene->getCanvasCurrent()->getCenter();
    osg::Vec3f N = mScene->getCanvasCurrent()->getNormal();

    // algorithm for distance between skew lines:
    //http://www2.washjeff.edu/users/mwoltermann/Dorrie/69.pdf
    // For two points P1 and P2 on skew lines;
    // and d - the direction vector from P1 to P2;
    // u1 and u2 - unit direction vectors for the lines
    osg::Vec3f P1 = C;
    osg::Vec3f P2 = nearPoint;
    osg::Vec3f d = P2 - P1;
    osg::Vec3f u1 = N;
    u1.normalize();
    osg::Vec3f u2 = farPoint - nearPoint;
    u2.normalize();
    osg::Vec3f u3 = u1^u2;

    if (std::fabs(u3.x())<=dureu::EPSILON && std::fabs(u3.y())<=dureu::EPSILON && std::fabs(u3.z())<=dureu::EPSILON){
        std::cerr << "getRaytraceNormalProjection(): cast ray and normal are almost parallel. To resolve, change the camera view." << std::endl;
        return false;
    }

    // X1 and X2 are the closest points on lines
    // we want to find X1 (u1 corresponds to normal)
    // solving the linear equation in r1 and r2: Xi = Pi + ri*ui
    // we are only interested in X1 so we only solve for r1.
    float a1 = u1*u1, b1 = u1*u2, c1 = u1*d;
    float a2 = u1*u2, b2 = u2*u2, c2 = u2*d;
    assert((std::fabs(b1) > dureu::EPSILON)); // denominator
    assert(a2!=-1 && a2!=1); // lines are not parallel and we already checked for that
    double r1 = (c2 - b2*c1/b1)/(a2-b2*a1/b1);
    osg::Vec3f X1 = P1 + u1*r1;
    XC = X1 - C;
    return true;
}