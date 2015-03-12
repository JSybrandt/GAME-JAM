//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================


#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Line.h"
#include "Quad.h"
#include "QuadObject.h"
#include <d3dx9math.h>
#include "LineObject.h"



class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void onResize();
	void updateScene(float dt);
	void drawScene(); 

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Quad quad1;
	Quad quad2;
	QuadObject end;
	Line line;
	Box mBox, redBox, greenBox;
	GameObject gameObject1, gameObject2, gameObject3, spinner;
	LineObject xLine, yLine, zLine;

	GameObject walls[100];

	GameObject fruits[100];

	float spinAmount;

	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	//my addition
	ID3D10EffectVariable* mfxFLIPVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

	float mTheta;
	float mPhi;

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(PI*0.25f)
{
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);
}

void ColoredCubeApp::initApp()
{
	D3DApp::initApp();
	
	mBox.init(md3dDevice, 1.0f, WHITE);
	redBox.init(md3dDevice, 1.0f, RED);
	greenBox.init(md3dDevice, 1.0f, GREEN);
	line.init(md3dDevice, 10.0f, GREEN);

	xLine.init(&line, Vector3(0,0,0), 5);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&line, Vector3(0,0,0), 5);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(ToRadian(90));
	zLine.init(&line, Vector3(0,0,0), 5);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(ToRadian(90));

	quad1.init(md3dDevice, Vector3(10,10,10), CYAN);
	quad1.setPosition(Vector3(0,-1.2,0));
	quad2.init(md3dDevice, Vector3(100,1,100), BLUE);
	quad2.setPosition(Vector3(0,0,1000));
	quad2.setRotXAngle(90);
	
	end.init(&quad2, 1, quad2.getPosition(), Vector3(0,0,0), 0, 1000);
	end.setAngleX(-90);

	spinAmount = 0;
	spinner.init(&redBox, 0, Vector3(0,4,0), Vector3(0,0,0), 0,1);

	gameObject1.init(&mBox, sqrt(2.0f), Vector3(0,0,0), Vector3(0,0,0), 0,1);
	gameObject2.init(&redBox, sqrt(2.0f), Vector3(4,0,0), Vector3(0,0,0), 0,1);
	gameObject3.init(&redBox, sqrt(2.0f), Vector3(-4,0,0), Vector3(0,0,0), 0,1);

	for(int i = 0 ; i < 100; i++)
	{
		walls[i].init(&redBox,sqrt(2.0f),Vector3(rand()%100-50,0,rand()%1000-50),Vector3(0,0,0),0,1);
		fruits[i].init(&greenBox,sqrt(2.0f),Vector3(rand()%100-50,0,rand()%1000-50),Vector3(0,0,0),0,1);
	}


	//gameObject1.setVelocity(D3DXVECTOR3(2,0,0));
	gameObject1.setRadius(2);
	buildFX();
	buildVertexLayouts();

}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::updateScene(float dt)
{
	D3DApp::updateScene(dt);
	gameObject1.update(dt);
	gameObject2.update(dt);
	gameObject3.update(dt);
	spinner.update(dt);
	xLine.update(dt);
	yLine.update(dt);
	zLine.update(dt);
	quad1.update(dt);
	quad2.update(dt);
	end.update(dt);

	for(int i = 0; i < 100; i++)
	{
		walls[i].update(dt);
		fruits[i].update(dt);
	}

	Vector3 input(0,0,0);

	// Update angles based on input to orbit camera around box.
	if(GetAsyncKeyState('W') & 0x8000)	input.z+=1;
	if(GetAsyncKeyState('A') & 0x8000)	input.x-=1;
	if(GetAsyncKeyState('S') & 0x8000)	input.z-=1;
	if(GetAsyncKeyState('D') & 0x8000)	input.x+=1;

	D3DXVec3Normalize(&input,&input);

	input *= 500;

	static bool collidingLastFrame = false;
	bool collidingThisFrame = false;

	for(int i = 0; i < 100; i++)
	{
		if(gameObject1.collided(&walls[i]))
			collidingThisFrame = true;
		if(gameObject1.collided(&fruits[i]))
			fruits[i].setInActive();
	}

	if(gameObject1.collided(&quad2)) {
		collidingThisFrame = true;
		gameObject1.setVelocity(Vector3(0,0,0));
	}

	if(collidingThisFrame)
	{
		if(!collidingLastFrame)
			gameObject1.setVelocity(gameObject1.getVelocity()*-1);
		collidingLastFrame = true;
	}
	else 
		collidingLastFrame = false;

	if(!collidingLastFrame)
		gameObject1.setVelocity(input);




// COLLISION DETECTION HERE
	spinAmount += dt ;
	if (ToRadian(spinAmount*40)>2*PI)
		spinAmount = 0;


	// Restrict the angle mPhi.
	if( mPhi < 0.1f )	mPhi = 0.1f;
	if( mPhi > PI-0.1f)	mPhi = PI-0.1f;

	// Convert Spherical to Cartesian coordinates: mPhi measured from +y
	// and mTheta measured counterclockwise from -z.
	float x =  25.0f*sinf(mPhi)*sinf(mTheta);
	float z = -25.0f*sinf(mPhi)*cosf(mTheta);
	float y =  25.0f*cosf(mPhi);

	// Build the view matrix.
	D3DXVECTOR3 pos(0, 3, -15);
	pos += gameObject1.getPosition();
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	target = gameObject1.getPosition();
	D3DXVECTOR3 up(0.0f, 0.0f, 1.0f);
	D3DXMatrixLookAtLH(&mView, &pos, &target, &up);
}

void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();

	

	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);
    md3dDevice->IASetInputLayout(mVertexLayout);

	// set some variables for the shader
	int foo[1];
	foo[0] = 0;
	// set the point to the shader technique
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc(&techDesc);

	//setting the color flip variable in the shader
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));

	//draw the lines
	mWVP = xLine.getWorldMatrix()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	xLine.setMTech(mTech);
	xLine.draw();
	
	mWVP = yLine.getWorldMatrix() *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	yLine.setMTech(mTech);
	yLine.draw();

	mWVP = zLine.getWorldMatrix() *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	zLine.setMTech(mTech);
	zLine.draw();

	//draw the quad using the "old" method
	//compare how messy this is compared to the objectified geometry classes
	mWVP = quad1.getWorld()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
    mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
       quad1.draw();
    }

	mWVP = quad2.getWorld()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
    mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
       quad2.draw();
    }

	//draw the boxes
	mWVP = gameObject1.getWorldMatrix()  *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	gameObject1.setMTech(mTech);
	gameObject1.draw();

	mWVP = gameObject2.getWorldMatrix()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	foo[0] = 0;
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
	gameObject2.setMTech(mTech);
	gameObject2.draw();


	mWVP = gameObject3.getWorldMatrix()*mView*mProj;
	foo[0] = 0;
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
	mfxWVPVar->SetMatrix((float*)&mWVP);
	gameObject3.setMTech(mTech);
	gameObject3.draw();

	//mWVP = end.getWorldMatrix()*mView*mProj;
	//foo[0] = 0;
	//mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
	//mfxWVPVar->SetMatrix((float*)&mWVP);
	//end.setMTech(mTech);
	//end.draw();
 //   
	for(int i = 0; i < 100; i++)
	{
		mWVP = walls[i].getWorldMatrix()*mView*mProj;
		foo[0] = 0;
		mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
		mfxWVPVar->SetMatrix((float*)&mWVP);
		walls[i].setMTech(mTech);
		walls[i].draw();
	}

	for(int i = 0; i < 100; i++)
	{
		mWVP = fruits[i].getWorldMatrix()*mView*mProj;
		foo[0] = 0;
		mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
		mfxWVPVar->SetMatrix((float*)&mWVP);
		fruits[i].setMTech(mTech);
		fruits[i].draw();
	}

	//draw the spinning box
	if (ToRadian(spinAmount*40) > PI)
		foo[0] = 1;
	else
		foo[0] = 0;
	mfxFLIPVar->SetRawValue(&foo[0], 0, sizeof(int));
	Matrix spin;
	RotateY(&spin, ToRadian(spinAmount*40));
	Matrix translate;
	Translate(&translate, 5, 0, 0);
	mWVP = spinner.getWorldMatrix() *translate * spin  *mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	spinner.setMTech(mTech);
	spinner.draw();


	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);
	mSwapChain->Present(0, 0);
}

void ColoredCubeApp::buildFX()
{
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"color.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("ColorTech");
	
	mfxWVPVar = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxFLIPVar = mFX->GetVariableByName("flip");

}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 2, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}
 