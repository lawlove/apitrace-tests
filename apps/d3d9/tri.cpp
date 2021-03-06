/**************************************************************************
 *
 * Copyright 2012 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include <windows.h>

#include <d3d9.h>


static IDirect3D9 * g_pD3D = NULL;
static IDirect3DDevice9 * g_pDevice = NULL;
static D3DPRESENT_PARAMETERS g_PresentationParameters;


int
main(int argc, char *argv[])
{
    HRESULT hr;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        DefWindowProc,
        0,
        0,
        hInstance,
        NULL,
        NULL,
        NULL,
        NULL,
        "SimpleDX9",
        NULL
    };
    RegisterClassEx(&wc);

    const int WindowWidth = 250;
    const int WindowHeight = 250;
    BOOL Windowed = TRUE;

    DWORD dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW;

    RECT rect = {0, 0, WindowWidth, WindowHeight};
    AdjustWindowRect(&rect, dwStyle, FALSE);

    HWND hWnd = CreateWindow(wc.lpszClassName,
                             "Simple example using DirectX9",
                             dwStyle,
                             CW_USEDEFAULT, CW_USEDEFAULT,
                             rect.right - rect.left,
                             rect.bottom - rect.top,
                             NULL,
                             NULL,
                             hInstance,
                             NULL);
    if (!hWnd) {
        return 1;
    }

    ShowWindow(hWnd, SW_SHOW);

    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D) {
        return 1;
    }

    D3DCAPS9 caps;
    hr = g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);
    if (FAILED(hr)) {
       return 1;
    }

    DWORD dwBehaviorFlags;
    if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 ||
        caps.VertexShaderVersion < D3DVS_VERSION(1, 1)) {
       dwBehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    } else {
       dwBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
    }

    ZeroMemory(&g_PresentationParameters, sizeof g_PresentationParameters);
    g_PresentationParameters.Windowed = Windowed;
    if (!Windowed) {
        g_PresentationParameters.BackBufferWidth = WindowWidth;
        g_PresentationParameters.BackBufferHeight = WindowHeight;
    }
    g_PresentationParameters.BackBufferCount = 1;
    g_PresentationParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    if (!Windowed) {
        g_PresentationParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
    } else {
        g_PresentationParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    }
    g_PresentationParameters.hDeviceWindow = hWnd;

    g_PresentationParameters.EnableAutoDepthStencil = FALSE;
    g_PresentationParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
                              D3DDEVTYPE_HAL,
                              hWnd,
                              dwBehaviorFlags,
                              &g_PresentationParameters,
                              &g_pDevice);
    if (FAILED(hr)) {
        g_pD3D->Release();
        g_pD3D = NULL;
        return 1;
    }

    struct Vertex {
        float x, y, z;
        DWORD color;
    };


    D3DCOLOR clearColor = D3DCOLOR_COLORVALUE(0.3f, 0.1f, 0.3f, 1.0f);
    g_pDevice->Clear(0, NULL, D3DCLEAR_TARGET, clearColor, 1.0f, 0);
    g_pDevice->BeginScene();

    g_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    static const Vertex vertices[] = {
        { -0.9f, -0.9f, 0.5f, D3DCOLOR_COLORVALUE(0.8f, 0.0f, 0.0f, 0.1f) },
        {  0.9f, -0.9f, 0.5f, D3DCOLOR_COLORVALUE(0.0f, 0.9f, 0.0f, 0.1f) },
        {  0.0f,  0.9f, 0.5f, D3DCOLOR_COLORVALUE(0.0f, 0.0f, 0.7f, 0.1f) },
    };

    g_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    g_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, vertices, sizeof(Vertex));

    g_pDevice->EndScene();

    g_pDevice->Present(NULL, NULL, NULL, NULL);


    g_pDevice->Release();
    g_pDevice = NULL;
    g_pD3D->Release();
    g_pD3D = NULL;

    DestroyWindow(hWnd);

    return 0;
}

