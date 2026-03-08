using System;

namespace NodeCanvasGL.Net;

public sealed class Engine : IDisposable
{
    private IntPtr _native;

    public Engine()
    {
        _native = NativeMethods.nc_engine_create();
    }

    public uint CreateNode(string nodeType, float x, float y)
    {
        return NativeMethods.nc_create_node(_native, nodeType, x, y);
    }

    public bool Connect(uint fromNode, uint fromConnector, uint toNode, uint toConnector)
    {
        return NativeMethods.nc_connect(_native, fromNode, fromConnector, toNode, toConnector);
    }

    public void Dispose()
    {
        if (_native == IntPtr.Zero)
        {
            return;
        }

        NativeMethods.nc_engine_destroy(_native);
        _native = IntPtr.Zero;
        GC.SuppressFinalize(this);
    }
}
