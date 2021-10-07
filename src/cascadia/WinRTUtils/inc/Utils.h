// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

// Method Description:
// - Scales a Rect based on a scale factor
// Arguments:
// - rect: Rect to scale by scale
// - scale: amount to scale rect by
// Return Value:
// - Rect scaled by scale
inline winrt::Windows::Foundation::Rect ScaleRect(winrt::Windows::Foundation::Rect rect, double scale)
{
    const auto scaleLocal = base::ClampedNumeric<float>(scale);
    rect.X = base::ClampMul(rect.X, scaleLocal);
    rect.Y = base::ClampMul(rect.Y, scaleLocal);
    rect.Width = base::ClampMul(rect.Width, scaleLocal);
    rect.Height = base::ClampMul(rect.Height, scaleLocal);
    return rect;
}

// Function Description:
// - This function presents a File Open "common dialog" and returns its selected file asynchronously.
// Parameters:
// - customize: A lambda that receives an IFileDialog* to customize.
// Return value:
// (async) path to the selected item.
template<typename TLambda>
winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> FilePicker(HWND parentHwnd, bool saveDialog, TLambda&& customize)
{
    auto fileDialog{ saveDialog ? winrt::create_instance<IFileDialog>(CLSID_FileSaveDialog) :
                                  winrt::create_instance<IFileDialog>(CLSID_FileOpenDialog) };
    DWORD flags{};
    THROW_IF_FAILED(fileDialog->GetOptions(&flags));
    THROW_IF_FAILED(fileDialog->SetOptions(flags | FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR | FOS_DONTADDTORECENT)); // filesystem objects only; no recent places
    customize(fileDialog.get());

    const auto hr{ fileDialog->Show(parentHwnd) };
    if (!SUCCEEDED(hr))
    {
        if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
        {
            co_return winrt::hstring{};
        }
        THROW_HR(hr);
    }

    winrt::com_ptr<IShellItem> result;
    THROW_IF_FAILED(fileDialog->GetResult(result.put()));

    wil::unique_cotaskmem_string filePath;
    THROW_IF_FAILED(result->GetDisplayName(SIGDN_FILESYSPATH, &filePath));

    co_return winrt::hstring{ filePath.get() };
}

template<typename TLambda>
winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> OpenFilePicker(HWND parentHwnd, TLambda&& customize)
{
    return FilePicker(parentHwnd, false, customize);
}
template<typename TLambda>
winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> SaveFilePicker(HWND parentHwnd, TLambda&& customize)
{
    return FilePicker(parentHwnd, true, customize);
}

winrt::Windows::Foundation::IAsyncOperation<winrt::hstring> OpenImagePicker(HWND parentHwnd);
