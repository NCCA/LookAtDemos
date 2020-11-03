#include <QMouseEvent>
#include <QGuiApplication>
#include "NGLScene.h"
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/NGLStream.h>



NGLScene::NGLScene()
{
  setTitle("Using ngl::lookAt() and ngl::perspective()");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL( int _w, int _h )
{
  m_win.width  = static_cast<int>( _w *devicePixelRatio() );
  m_win.height = static_cast<int>( _h *devicePixelRatio() );
}


void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::initialize();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
   // now to load the shader and set the values
  ngl::ShaderLib::use("nglDiffuseShader");

  ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,1.0f,1.0f);
  ngl::ShaderLib::setUniform("lightPos",1.0f,1.0f,1.0f);
  ngl::ShaderLib::setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);

  ngl::Vec3 from(2,2,2);
  ngl::Vec3 to(0,0,0);
  ngl::Vec3 up(0,1,0);
  m_view=ngl::lookAt(from,to,up);
  std::cout<<m_view<<"\n";
  int w=this->size().width();
  int h=this->size().height();

  m_projection=ngl::perspective(45,static_cast<float>(w)/h,0.05f,350.0f);
  // as re-size is not explicitly called we need to do this.
  glViewport(0,0,width(),height());

}


void NGLScene::loadMatricesToShader()
{
  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_mouseGlobalTX;
  MV=  m_view*M;
  MVP= m_projection*MV;
  normalMatrix=MV;
  normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MVP",MVP);
  ngl::ShaderLib::setUniform("normalMatrix",normalMatrix);
 }

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  ngl::ShaderLib::use("nglDiffuseShader");

  // Rotation based on the mouse position for our global transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_win.spinXFace);
  rotY.rotateY(m_win.spinYFace);
  // multiply the rotations
  m_mouseGlobalTX=rotY*rotX;
  // add the translations
  m_mouseGlobalTX.m_m[3][0] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[3][1] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[3][2] = m_modelPos.m_z;

  loadMatricesToShader();
  ngl::VAOPrimitives::draw("troll");
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : showFullScreen(); break;
  // show windowed
  case Qt::Key_N : showNormal(); break;
  case Qt::Key_Space :
        m_win.spinXFace=0;
        m_win.spinYFace=0;
        m_modelPos.set(ngl::Vec3::zero());
      break;
  default : break;
  }
  // finally update the GLWindow and re-draw
  //if (isExposed())
    update();
}
