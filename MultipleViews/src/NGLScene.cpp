#include <QMouseEvent>
#include <QGuiApplication>
#include "NGLScene.h"
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>


//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for x/y translation with mouse movement
//----------------------------------------------------------------------------------------------------------------------
constexpr float INCREMENT=0.01f;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the increment for the wheel zoom
//----------------------------------------------------------------------------------------------------------------------
constexpr float ZOOM=0.1f;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the offset for full window mode mouse data
//----------------------------------------------------------------------------------------------------------------------
constexpr static size_t FULLOFFSET=4;

NGLScene::NGLScene()
{
  for(int i=0; i<5; ++i)
  for(auto &panel : m_panelMouseInfo)
  {
    panel.m_spinXFace=0;
    panel.m_spinYFace=0;
    panel.m_rotate=false;
  }

  // now we need to set the scales for the ortho windos
  m_panelMouseInfo[static_cast<size_t>(Window::FRONT)].m_modelPos.set(0.0f,0.0f,1.0f);
  m_panelMouseInfo[static_cast<size_t>(Window::SIDE)].m_modelPos.set(0.0f,0.0f,1.0f);
  m_panelMouseInfo[static_cast<size_t>(Window::TOP)].m_modelPos.set(0.0f,0.0f,1.0f);


  setTitle("Multiple Views");


}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}



void NGLScene::resizeGL(int _w , int _h)
{
  // take into account device ratio later in the resize to be safe
  m_width=_w;
  m_height=_h;
}

void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::initialize();
  glClearColor(0.4f, 0.4f, 0.4f, 1.0f);			   // Grey Background
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);
  // now to load the shader and set the values
  // grab an instance of shader manager
  ngl::ShaderLib::use("nglDiffuseShader");

  ngl::ShaderLib::setUniform("Colour",1.0f,1.0f,1.0f,1.0f);
  ngl::ShaderLib::setUniform("lightPos",1.0f,1.0f,1.0f);
  ngl::ShaderLib::setUniform("lightDiffuse",1.0f,1.0f,1.0f,1.0f);
  // get the VBO instance and draw the built in teapot
  ngl::VAOPrimitives::createLineGrid("grid",40,40,40);


}

void NGLScene::frameActive()
{
  auto win = static_cast<size_t>(getActiveQuadrant());
  if(m_activeWindow != Window::ALL)
  {
    win= FULLOFFSET;
  }
  m_panelMouseInfo[win].m_modelPos.set(0,0,1);
}

void NGLScene::loadMatricesToShader()
{

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  M=m_globalTransform.getMatrix();
  MV=  m_globalTransform.getMatrix()*m_view;
  MVP= m_projection*m_view*M;
  normalMatrix=MV;
  normalMatrix.inverse().transpose();
  ngl::ShaderLib::setUniform("MVP",MVP);
  ngl::ShaderLib::setUniform("normalMatrix",normalMatrix);
 }


void NGLScene::top(Mode _m)
{
  // grab an instance of the shader manager
  ngl::ShaderLib::use("nglDiffuseShader");

  // Rotation based on the mouse position for our global transform
  size_t win=FULLOFFSET;
  if(_m == Mode::PANEL)
  {
    win=static_cast<size_t>(Window::TOP);
  }


  // first draw a top  persp // front //side
  ngl::Vec3 from(0.0f,2.0f,0.0f);
  ngl::Vec3 to(0.0f,0.0f,0.0f);
  ngl::Vec3 up(0.0f,0.0f,-1.0f);
  m_globalTransform.reset();
  {
    /// a top view (left upper)
    m_view=ngl::lookAt(from,to,up);
    m_projection=ngl::ortho(-1.0f,1.0f,-1.0f,1.0f, 0.1f, 100.0f);
    // x,y w/h
    if(_m==Mode::PANEL)
    {
      glViewport (0,static_cast<int>((m_height/2.0)*devicePixelRatio()), static_cast<GLsizei>((m_width/2.0)*devicePixelRatio()), static_cast<GLsizei>((m_height/2.0)*devicePixelRatio()));
    }
    else
    {
      glViewport(0,0,static_cast<GLsizei>(m_width*devicePixelRatio()),static_cast<GLsizei>(m_height*devicePixelRatio()));
    }
      // draw
    // in this case set to an identity
    ngl::Vec3 p=m_panelMouseInfo[win].m_modelPos;
    m_globalTransform.setPosition(p.m_x,0.0f,-p.m_y);
    m_globalTransform.setScale(p.m_z,p.m_z,p.m_z);

    loadMatricesToShader();
    ngl::VAOPrimitives::draw("troll");
    m_globalTransform.addPosition(0.0f,-1.0f,0.0f);
    loadMatricesToShader();
    ngl::VAOPrimitives::draw("grid");
  }

}

void NGLScene::side(Mode _m)
{
  // grab an instance of the shader manager
  ngl::ShaderLib::use("nglDiffuseShader");

  // Rotation based on the mouse position for our global transform
  size_t win=FULLOFFSET;
  if(_m == Mode::PANEL)
  {
    win=static_cast<size_t>(Window::SIDE);
  }


  // first draw a top  persp // front //side
  ngl::Vec3 from(0.0f,2.0f,0.0f);
  ngl::Vec3 to(0.0f,0.0f,0.0f);
  ngl::Vec3 up(0.0f,0.0f,-1.0f);
  /// a side view (bottom right)
  m_globalTransform.reset();
  {
    from.set(2.0f,0.0f,0.0f);
    up.set(0.0f,1.0f,0.0f);
    m_view=ngl::lookAt(from,to,up);
    m_projection=ngl::ortho(-1.0f,1.0f,-1.0f,1.0f, 0.1f, 100.0f);
    // x,y w/h
    if(_m==Mode::PANEL)
    {
      glViewport (static_cast<int>((m_width/2.0)*devicePixelRatio()), 0, static_cast<GLsizei>((m_width/2.0)*devicePixelRatio()), static_cast<GLsizei>((m_height/2.0)*devicePixelRatio()));
    }
    else
    {
      glViewport(0,0,static_cast<GLsizei>(m_width*devicePixelRatio()),static_cast<GLsizei>(m_height*devicePixelRatio()));
    }

    // draw
    ngl::Vec3 p=m_panelMouseInfo[win].m_modelPos;
    m_globalTransform.setPosition(0.0f,p.m_y,-p.m_x);
    m_globalTransform.setScale(p.m_z,p.m_z,p.m_z);


    loadMatricesToShader();
    ngl::VAOPrimitives::draw("troll");
    m_globalTransform.setRotation(90.0f,90.0f,0.0f);
    m_globalTransform.addPosition(0.0f,0.0f,2.0f);
    loadMatricesToShader();
    ngl::VAOPrimitives::draw("grid");
  }



}
void NGLScene::persp(Mode _m)
{
  ngl::ShaderLib::use("nglDiffuseShader");

  // Rotation based on the mouse position for our global transform
  // 4 is the panel full screen mode
  size_t win=FULLOFFSET;
  if(_m == Mode::PANEL)
  {
    win=static_cast<size_t>(Window::PERSP);
  }
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX(m_panelMouseInfo[win].m_spinXFace);
  rotY.rotateY(m_panelMouseInfo[win].m_spinYFace);
  // multiply the rotations
  ngl::Mat4 final=rotY*rotX;
  // add the translations
  final.m_m[3][0] = m_panelMouseInfo[win].m_modelPos.m_x;
  final.m_m[3][1] = m_panelMouseInfo[win].m_modelPos.m_y;
  final.m_m[3][2] = m_panelMouseInfo[win].m_modelPos.m_z;
  // set this in the TX stack


  // first draw a top  persp // front //side
  ngl::Vec3 from(0.0f,2.0f,0.0f);
  ngl::Vec3 to(0.0f,0.0f,0.0f);
  ngl::Vec3 up(0.0f,0.0f,-1.0f);
  m_globalTransform.reset();
  {
    /// a perspective view (right upper)
    //first set the global mouse rotation for this one
    m_globalTransform.setMatrix(final);
    from.set(0.0f,1.0f,1.0f);
    up.set(0.0f,1.0f,0.0f);
    m_view=ngl::lookAt(from,to,up);
    m_projection=ngl::perspective(45.0f,static_cast<float>(m_width)/m_height,0.01f,100.0f);
    // x,y w/h
    if(_m==Mode::PANEL)
    {
      glViewport (static_cast<int>((m_width/2.0)*devicePixelRatio()), static_cast<int>((m_height/2.0)*devicePixelRatio()),
                  static_cast<GLsizei>((m_width/2.0)*devicePixelRatio()), static_cast<GLsizei>((m_height/2.0)*devicePixelRatio()));
    }
    else
    {
      glViewport(0,0,static_cast<GLsizei>(m_width*devicePixelRatio()),static_cast<GLsizei>(m_height*devicePixelRatio()));
    }

    // draw
    loadMatricesToShader();
    ngl::VAOPrimitives::draw("troll");
    // now we need to add an offset to the y position to draw the grid,
    // easiest way is to just modify the matrix directly
    final.m_m[3][1]-=0.8f;
    m_globalTransform.setMatrix(final);

    //m_globalTransform.setPosition(0,-0.55,0);
    loadMatricesToShader();
    ngl::VAOPrimitives::draw("grid");
  }

}
void NGLScene::front(Mode _m)
{
  ngl::ShaderLib::use("nglDiffuseShader");

  size_t win=FULLOFFSET;
  if(_m == Mode::PANEL)
  {
    win=static_cast<size_t>(Window::FRONT);
  }


  // first draw a top  persp // front //side
  ngl::Vec3 from(0.0f,2.0f,0.0f);
  ngl::Vec3 to(0.0f,0.0f,0.0f);
  ngl::Vec3 up(0.0f,0.0f,-1.0f);
  /// a front view (bottom left)
  m_globalTransform.reset();
  {
    from.set(0.0f,0.0f,2.0f);
    up.set(0.0f,1.0f,0.0f);
    m_view=ngl::lookAt(from,to,up);
    m_projection=ngl::ortho(-1.0f,1.0f,-1.0f,1.0f, 0.01f, 200.0f);
    // x,y w/h
    if(_m==Mode::PANEL)
    {
      glViewport (0,0, static_cast<GLsizei>((m_width/2.0)*devicePixelRatio()), static_cast<GLsizei>((m_height/2.0)*devicePixelRatio()));
    }
    else
    {
      glViewport(0,0,static_cast<GLsizei>(m_width*devicePixelRatio()),static_cast<GLsizei>(m_height*devicePixelRatio()));
    }
    // draw
    // in this case set to an identity
    ngl::Vec3 p=m_panelMouseInfo[win].m_modelPos;
    m_globalTransform.setPosition(p.m_x,p.m_y,0.0f);
    m_globalTransform.setScale(p.m_z,p.m_z,p.m_z);
    loadMatricesToShader();
    ngl::VAOPrimitives::draw("troll");
    m_globalTransform.setRotation(90.0f,0.0f,0.0f);
    m_globalTransform.addPosition(0.0f,0.0f,-1.0f);
    loadMatricesToShader();
    ngl::VAOPrimitives::draw("grid");
  }

}


NGLScene::Window NGLScene::getActiveQuadrant() const
{
  // find where the  mouse is and set quadrant
  if( (m_mouseX <m_width/2) && (m_mouseY<m_height/2) )
  {
    return Window::TOP;
  }
  else if( (m_mouseX >=m_width/2) && (m_mouseY<m_height/2) )
  {
    return Window::PERSP;
  }
  else if( (m_mouseX <=m_width/2) && (m_mouseY>m_height/2) )
  {
    return Window::FRONT;
  }
  else if( (m_mouseX >=m_width/2) && (m_mouseY>m_height/2) )
  {
    return Window::SIDE;
  }
  else // should never get here but stops warning
  {
    return Window::ALL;
  }
}


void NGLScene::toggleWindow()
{
  if(m_activeWindow ==Window::ALL)
  {
    m_activeWindow=getActiveQuadrant();
    //store mouse info as we have gone fullscreen
    m_panelMouseInfo[FULLOFFSET].m_modelPos = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_modelPos;
    m_panelMouseInfo[FULLOFFSET].m_origX = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_origX;
    m_panelMouseInfo[FULLOFFSET].m_origY = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_origY;

    m_panelMouseInfo[FULLOFFSET].m_spinXFace = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_spinXFace;
    m_panelMouseInfo[FULLOFFSET].m_spinYFace = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_spinYFace;
    m_panelMouseInfo[FULLOFFSET].m_origXPos = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_origXPos;
    m_panelMouseInfo[FULLOFFSET].m_origYPos = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_origYPos;
    m_panelMouseInfo[FULLOFFSET].m_rotate = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_rotate;
    m_panelMouseInfo[FULLOFFSET].m_translate = m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_translate;
  }
  else
  {
    // store info as we are minimised
    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_modelPos = m_panelMouseInfo[FULLOFFSET].m_modelPos;
    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_origX = m_panelMouseInfo[FULLOFFSET].m_origX;
    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_origY = m_panelMouseInfo[FULLOFFSET].m_origY;

    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_spinXFace = m_panelMouseInfo[FULLOFFSET].m_spinXFace;
    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_spinYFace = m_panelMouseInfo[FULLOFFSET].m_spinYFace;
    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_origXPos = m_panelMouseInfo[FULLOFFSET].m_origXPos;
    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_origYPos = m_panelMouseInfo[FULLOFFSET].m_origYPos;
    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_rotate = m_panelMouseInfo[FULLOFFSET].m_rotate;
    m_panelMouseInfo[static_cast<size_t>(m_activeWindow)].m_translate = m_panelMouseInfo[FULLOFFSET].m_translate;
    m_activeWindow=Window::ALL;
  }

}


void NGLScene::paintGL()
{
  // clear the screen and depth buffer
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   switch(m_activeWindow)
   {
     case Window::ALL :
     {
       front(Mode::PANEL);
       side(Mode::PANEL);
       top(Mode::PANEL);
       persp(Mode::PANEL);
       break;
     }
     case Window::FRONT : {front(Mode::FULLSCREEN); break; }
     case Window::SIDE : {side(Mode::FULLSCREEN); break; }
     case Window::TOP : {top(Mode::FULLSCREEN); break; }
     case Window::PERSP : {persp(Mode::FULLSCREEN); break; }

   }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent (QMouseEvent * _event)
{
  m_mouseX=_event->x();
  m_mouseY=_event->y();
  size_t win;
  if(m_activeWindow ==Window::ALL)
    win=static_cast<size_t>(getActiveQuadrant());
  else
    win=static_cast<size_t>(FULLOFFSET);
  std::cout<<"moveEvent "<<win<<'\n';
  // note the method buttons() is the button state when event was called
  // this is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if(m_panelMouseInfo[win].m_rotate && _event->buttons() == Qt::LeftButton)
  {

    int diffx=_event->x()-m_panelMouseInfo[win].m_origX;
    int diffy=_event->y()-m_panelMouseInfo[win].m_origY;
    m_panelMouseInfo[win].m_spinXFace +=  0.5f * diffy;
    m_panelMouseInfo[win].m_spinYFace +=  0.5f * diffx;
    m_panelMouseInfo[win].m_origX = _event->x();
    m_panelMouseInfo[win].m_origY = _event->y();
    update();

	}
	// right mouse translate code
	else if(m_panelMouseInfo[win].m_translate && _event->buttons() == Qt::RightButton)
	{
    int diffX =(_event->x() - m_panelMouseInfo[win].m_origXPos);
    int diffY =(_event->y() - m_panelMouseInfo[win].m_origYPos);
		m_panelMouseInfo[win].m_origXPos=_event->x();
		m_panelMouseInfo[win].m_origYPos=_event->y();
		m_panelMouseInfo[win].m_modelPos.m_x += INCREMENT * diffX;
		m_panelMouseInfo[win].m_modelPos.m_y -= INCREMENT * diffY;
		update();

	}
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent ( QMouseEvent * _event)
{
  size_t win;
	if(m_activeWindow ==Window::ALL)
    win=static_cast<size_t>(getActiveQuadrant());
	else
    win=static_cast<size_t>(FULLOFFSET);
  std::cout<<"Mouse Presee "<<win<<'\n';
  // this method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  if(_event->button() == Qt::LeftButton)
  {
    m_panelMouseInfo[win].m_origX = _event->x();
    m_panelMouseInfo[win].m_origY = _event->y();
    m_panelMouseInfo[win].m_rotate =true;
  }
  // right mouse translate mode
  else if(_event->button() == Qt::RightButton)
  {
    m_panelMouseInfo[win].m_origXPos = _event->x();
    m_panelMouseInfo[win].m_origYPos = _event->y();
    m_panelMouseInfo[win].m_translate=true;
  }

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent ( QMouseEvent * _event )
{
  // this event is called when the mouse button is released
  // we then set Rotate to false
  size_t win;
  if(m_activeWindow ==Window::ALL)
    win=static_cast<size_t>(getActiveQuadrant());
  else
    win=static_cast<size_t>(FULLOFFSET);

  std::cout<<"Mouse release "<<win<<'\n';

  if (_event->button() == Qt::LeftButton)
  {
    m_panelMouseInfo[win].m_rotate=false;
  }
  // right mouse translate mode
  if (_event->button() == Qt::RightButton)
  {
    m_panelMouseInfo[win].m_translate=false;
  }
 }

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent(QWheelEvent *_event)
{

  size_t win;
	if(m_activeWindow ==Window::ALL)
    win=static_cast<size_t>(getActiveQuadrant());
	else
    win=static_cast<size_t>(FULLOFFSET);
  std::cout<<"wheel "<<win<<'\n';
	// check the diff of the wheel position (0 means no change)
	if(_event->delta() > 0)
	{
		m_panelMouseInfo[win].m_modelPos.m_z+=ZOOM;
	}
	else if(_event->delta() <0 )
	{
		m_panelMouseInfo[win].m_modelPos.m_z-=ZOOM;
	}
	update();
}
//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the NGLScene
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  // turn on wirframe rendering
  case Qt::Key_W : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
  // turn off wire frame
  case Qt::Key_S : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
  // show full screen
  case Qt::Key_F : frameActive(); break;
    // show windowed
    case Qt::Key_Space : toggleWindow(); break;
  default : break;
  }
  update();
}
