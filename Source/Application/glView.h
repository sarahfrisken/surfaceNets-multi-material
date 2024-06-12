//
// glView.h
//
// OpenGL View 
//  + Controls the OpenGL window and OpenGl rendering of a SurfaceNet model

#ifndef GLVIEW_H
#define GLVIEW_H

#include "MMSurfaceNet.h"
#include "MMGeometryGL.h"

#include <QMatrix4x4>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShaderProgram>

class GLView : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit GLView(QWidget *parent = 0);
	~GLView();

	QSize minimumSizeHint() const override { return QSize(50, 50); }
	QSize sizeHint() const override { return QSize(400, 400); }

	void updateRenderParameters(std::vector<QColor> colors, std::vector<bool> isVisible);
	void makeGeometry(MMSurfaceNet *surfaceNet);
	void reset();

protected:
	void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QMouseEvent *e) Q_DECL_OVERRIDE;

	void initializeGL() Q_DECL_OVERRIDE;
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;

private:
	MMGeometryGL *m_pGeometry;
	int m_numIndices;
	float m_origin[3];
	float m_size[3];

	void cleanupBufers();

	// GL rendering utilities
	QOpenGLShaderProgram *program;
	void initShaders(const char *vertexShader, const char *fragmentShader);

	QMatrix4x4 modelMatrix;
	QMatrix4x4 viewMatrix;
	QMatrix4x4 projMatrix;
	void render();

	QOpenGLBuffer *makeIndexBuffer(GLuint *indices, int numIndices);
	QOpenGLBuffer *makeVertexBuffer(GLfloat *vertices, int numVertices);
	void resetVertexBuffer(QOpenGLBuffer *buffer, GLfloat *vertices, int numVertices);

	// SurfaceNet 
	int maxNumColors = 256;
	float surfaceColorArray[4 * 256];

	QOpenGLBuffer *vertexBuffer = 0;
	QOpenGLBuffer *indexBuffer = 0;

	void drawGeometry(QOpenGLShaderProgram *program);

	// 3D view control
	float m_scale;
	float m_dx;
	float m_dy;
	float m_dz;
	float m_xRot;
	float m_yRot;
	float m_zRot;
	void setXRotation(int angle);
	void setYRotation(int angle);
	void setZRotation(int angle);
	void setScale(float scale);
	void setTranslate(float dx, float dy, float dz);
	QPoint m_lastMousePos;
};

#endif