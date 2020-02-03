#include "UprisingEditor.h"
#include "Log.h"

IMPLEMENT_GAME_MODULE(FUprisingEditorModule, UprisingEditor);

#define LOCTEXT_NAMESPACE "UprisingEditor"

void FUprisingEditorModule::StartupModule()
{
	LOG(UprisingEditor, "Starting UprisingEditor Module...");
}

void FUprisingEditorModule::ShutdownModule()
{
	LOG(UprisingEditor, "Shutting down UprisingEditor Module...");
}

#undef LOCTEXT_NAMESPACE