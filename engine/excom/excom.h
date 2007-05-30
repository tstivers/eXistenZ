#pragma once

struct IUnknown;

struct IUnknown {
	virtual void* QueryInterface(int inum) { return NULL };	
}

struct IEventHandler {
	virtual bool HandleEvent(Event* e);
}

struct IRenderable {
	virtual bool 
}