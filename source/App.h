/**
  \file App.h

  The G3D 10.00 default starter app is configured for OpenGL 4.1 and
  relatively recent GPUs.
 */
#ifndef APP_H
#define APP_H

#include <G3D/G3DAll.h>
#include "RayTracer.h"

/** \brief Application framework. */
class App : public GApp {
protected:

    /** Called from onInit */
    void makeGUI();

public:
    
    App(const GApp::Settings& settings = GApp::Settings());

    virtual void onInit() override;
    virtual void onAI() override;
    virtual void onNetwork() override;
    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) override;
    virtual void onPose(Array<shared_ptr<Surface> >& posed3D, Array<shared_ptr<Surface2D> >& posed2D) override;

    // You can override onGraphics if you want more control over the rendering loop.
    // virtual void onGraphics(RenderDevice* rd, Array<shared_ptr<Surface> >& surface, Array<shared_ptr<Surface2D> >& surface2D) override;

    virtual void onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) override;
    virtual void onGraphics2D(RenderDevice* rd, Array<shared_ptr<Surface2D> >& surface2D) override;

    virtual bool onEvent(const GEvent& e) override;
    virtual void onUserInput(UserInput* ui) override;
    virtual void onCleanup() override;

	void render();
	Vector2int32 getResolution() const;

private:
	//Cylinder options
	float	height;
	float	radius;
	//Resolution
	Array<GuiText>	resolutionList;
	int		resolutionID;
	//RayTracer options
	bool	fixedPrimitives;
	bool	multithreading;
	int		raysPerPixel;
	int		z_near;
	//RayTracer
	std::unique_ptr<RayTracer>	rayTracer;
	//Output image
	Vector2int32	resolution;
	std::shared_ptr<Image>	image;
	std::shared_ptr<Texture> result;
	//Time
	std::unique_ptr<Stopwatch>		sw;
	RealTime		renderTime;
	//Surfaces
	Array<shared_ptr<Surface>>	surfaces;
	Array<shared_ptr<Surface2D>> surfaces2D;
};

#endif // !APP_H
