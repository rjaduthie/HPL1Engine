/*
 * Copyright (C) 2006-2010 - Frictional Games
 *
 * This file is part of HPL1 Engine.
 *
 * HPL1 Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HPL1 Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with HPL1 Engine.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "impl/SqScript.h"
#include "system/LowLevelSystem.h"
#include "system/String.h"
#include "math/Math.h"
#include "impl/Platform.h"
#include <stdio.h>

namespace hpl {

	//////////////////////////////////////////////////////////////////////////
	// CONSTRUCTORS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	cSqScript::cSqScript(const tString& asName,asIScriptEngine *apScriptEngine,
							cScriptOutput *apScriptOutput, int alHandle)
		: iScript(asName)
	{
		mpScriptEngine = apScriptEngine;
		mpScriptOutput = apScriptOutput;
		mlHandle = alHandle;

		mpContext = mpScriptEngine->CreateContext();

		//Create a unique module name
		msModuleName = "Module_"+cString::ToString(cMath::RandRectl(0,1000000))+
						"_"+cString::ToString(mlHandle);
        mpModule = mpScriptEngine->GetModule(msModuleName.c_str(), asGM_CREATE_IF_NOT_EXISTS);
	}

	cSqScript::~cSqScript()
	{
		mpModule->Discard();
		mpContext->Release();
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PUBLIC METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	bool cSqScript::CreateFromFile(const tString& asFileName)
	{
		int lLength;
		char *pCharBuffer = LoadCharBuffer(asFileName,lLength);
		if(pCharBuffer==nullptr){
			Error("Couldn't load script '%s'!\n",asFileName.c_str());
			return false;
		}

		if(mpModule->AddScriptSection("main", pCharBuffer, lLength) < 0)
		{
			Error("Couldn't add script '%s'!\n",asFileName.c_str());
			hplDeleteArray(pCharBuffer);
			return false;
		}

		if(mpModule->Build() < 0)
		{
			Error("Couldn't build script '%s'!\n",asFileName.c_str());
			Log("------- SCRIPT OUTPUT BEGIN --------------------------\n");
			mpScriptOutput->Display();
			mpScriptOutput->Clear();
			Log("------- SCRIPT OUTPUT END ----------------------------\n");

			hplDeleteArray(pCharBuffer);
			return false;
		}
		mpScriptOutput->Clear();

		hplDeleteArray(pCharBuffer);
		return true;
	}

	//-----------------------------------------------------------------------

	int cSqScript::GetFuncHandle(const tString& asFunc)
	{
		return mpModule->GetFunctionByName(asFunc.c_str())->GetId();
	}

	//-----------------------------------------------------------------------

	void cSqScript::AddArg(const tString& asArg)
	{

	}

	//-----------------------------------------------------------------------

	bool cSqScript::Run(const tString& asFuncLine)
	{
	    asIScriptFunction *func = mpModule->GetFunctionByDecl(asFuncLine.c_str());
	    mpContext->Prepare(func);
	    if(mpContext->Execute() == asEXECUTION_FINISHED) {
	        asDWORD ret = mpContext->GetReturnDWord();
	    }
        mpContext->Release();
		return true;
	}

	//-----------------------------------------------------------------------

	bool cSqScript::Run(int alHandle)
	{
        asIScriptFunction *func = mpModule->GetFunctionByIndex(alHandle);

		mpContext->Prepare(func);

		/* Set all the args here */
        if(mpContext->Execute() == asEXECUTION_FINISHED) {
            asDWORD ret = mpContext->GetReturnDWord();
        }
        mpContext->Release();

		return true;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	char* cSqScript::LoadCharBuffer(const tString& asFileName, int& alLength)
	{
		FILE *pFile = fopen(asFileName.c_str(), "rb");
		if(pFile==nullptr){
			return nullptr;
		}

		int lLength = (int)Platform::FileLength(pFile);
		alLength = lLength;

		char *pBuffer = hplNewArray(char,lLength);
		fread(pBuffer, lLength, 1, pFile);

		fclose(pFile);

		return pBuffer;
	}

	//-----------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// STATIC PRIVATE METHODS
	//////////////////////////////////////////////////////////////////////////

	//-----------------------------------------------------------------------

	//-----------------------------------------------------------------------

}
