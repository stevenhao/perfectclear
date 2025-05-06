export async function initializeWasm() {
  try {
    (window as any).Module = {
      locateFile: function(path: string) {
        if (path.endsWith('.wasm')) return `/dist/${path}`;
        if (path.endsWith('.data')) return `/dist/${path}`;
        return path;
      }
    };

    await loadScript('/dist/perfectclear.js');
    await loadScript('/perfectclear-wasm.js');

    if ((window as any).loadWasmModule) {
      await (window as any).loadWasmModule();
      console.log('WASM module loaded successfully');
      return true;
    } else {
      console.error('loadWasmModule not available');
      return false;
    }
  } catch (error) {
    console.error('Failed to initialize WASM module:', error);
    return false;
  }
}

function loadScript(src: string): Promise<void> {
  return new Promise((resolve, reject) => {
    const script = document.createElement('script');
    script.src = src;
    script.onload = () => resolve();
    script.onerror = (e) => reject(new Error(`Failed to load script: ${src}`));
    document.head.appendChild(script);
  });
}
