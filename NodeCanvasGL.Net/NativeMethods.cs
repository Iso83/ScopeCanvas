using System;
using System.Runtime.InteropServices;

namespace NodeCanvasGL.Net;

internal static class NativeMethods
{
    [DllImport("nodecanvasgl")]
    internal static extern IntPtr nc_engine_create();

    [DllImport("nodecanvasgl")]
    internal static extern void nc_engine_destroy(IntPtr engine);

    [DllImport("nodecanvasgl")]
    internal static extern uint nc_create_node(
        IntPtr engine,
        string nodeType,
        float x,
        float y
    );

    [DllImport("nodecanvasgl")]
    internal static extern bool nc_connect(
        IntPtr engine,
        uint fromNode,
        uint fromConnector,
        uint toNode,
        uint toConnector
    );

    [DllImport("nodecanvasgl")]
    internal static extern void nc_render(IntPtr engine);
}
