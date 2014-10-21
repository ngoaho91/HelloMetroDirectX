#include "pch.h"
#include "Game.h"
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Popups;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Platform;

ref class App sealed : public IFrameworkView
{
private:
	bool windowClosed;
	Game game;
public:
	virtual void Initialize(CoreApplicationView^ app_view)
	{
		app_view->Activated += ref new TypedEventHandler
			<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);
		CoreApplication::Suspending +=
			ref new EventHandler<SuspendingEventArgs^>(this, &App::Suspending);
		CoreApplication::Resuming +=
			ref new EventHandler<Object^>(this, &App::Resuming);
	}
	void Suspending(Object^ Sender, SuspendingEventArgs^ Args)
	{
	}
	void Resuming(Object^ Sender, Object^ Args)
	{
	}
	
	virtual void SetWindow(CoreWindow^ window) 
	{
		window->PointerPressed += ref new TypedEventHandler
			<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerPressed);
		window->KeyDown += ref new TypedEventHandler
			<CoreWindow^, KeyEventArgs^>(this, &App::OnKeyDown);
		window->KeyUp += ref new TypedEventHandler
			<CoreWindow^, KeyEventArgs^>(this, &App::OnKeyUp);
		window->PointerWheelChanged += ref new TypedEventHandler
			<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerWheelChanged);
		window->Closed += ref new TypedEventHandler
			<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnClosed);
		window->SizeChanged += ref new TypedEventHandler
			<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnSizeChanged);
	}
	
	virtual void Load(String^ entry_point) {}
	virtual void Run()
	{
		game.Initialize();
		MessageDialog dialog("PRESS \"SPACE\" to toggle wireframe render\nPRESS \"B\" to toggle depth test\nCLICK on cubes to see they blink\nSNAP this window to see it pause/resume", "HELLO!");
		dialog.ShowAsync();
		CoreWindow^ window = CoreWindow::GetForCurrentThread();
		while (!windowClosed)
		{
			window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
			game.Update();
			game.Render();
		}
	}
	virtual void Uninitialize() {}

	void OnSizeChanged(CoreWindow^ window, WindowSizeChangedEventArgs^ e)
	{
		if (e->Size.Width < 600)
		{
			game.Pause();
		}
		else
		{
			game.Resume();
		}
	}
	void OnActivated(CoreApplicationView^ core_app_view, IActivatedEventArgs^ args)
	{
		CoreWindow^ window = CoreWindow::GetForCurrentThread();
		window->Activate();
	}
	void OnPointerPressed(CoreWindow^ window, PointerEventArgs^ args)
	{
		float x = args->CurrentPoint->Position.X;
		float y = args->CurrentPoint->Position.Y;
		float ndc_x = (x / window->Bounds.Width)*2.0f - 1.0f;
		float ndc_y = (y / window->Bounds.Height)*2.0f - 1.0f;
		int ret = game.ScreenPositionToObject(ndc_x, ndc_y);

		//MessageDialog dialog("Mouse pressed at " + ndc_x.ToString() + ", " + ndc_y.ToString()+", and intersects with cube number "+ret.ToString(), "Mouse pressed!");
		//dialog.ShowAsync();
	}
	void OnKeyDown(CoreWindow^ window, KeyEventArgs^ args)
	{
		if (args->VirtualKey == VirtualKey::A)
		{
			// do something...
		}
	}
	void OnKeyUp(CoreWindow^ window, KeyEventArgs^ args)
	{
		if (args->VirtualKey == VirtualKey::Space)
		{
			game.SwitchWireFrame();
		}
		if (args->VirtualKey == VirtualKey::B)
		{
			game.SwitchDepth();
		}
	}
	void OnPointerWheelChanged(CoreWindow^ sender, PointerEventArgs^ args)
	{
		int wheel = args->CurrentPoint->Properties->MouseWheelDelta;
	}
	void OnClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
	{
		windowClosed = true;
	}
};

ref class AppSource sealed : IFrameworkViewSource
{
public:
	virtual IFrameworkView^ CreateView()
	{
		return ref new App();
	}
};

[MTAThread]

int main(Array<String^>^ args)
{
	CoreApplication::Run(ref new AppSource());
	return 0;
}