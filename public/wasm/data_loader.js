async function fetchDataFile(filename) {
  const response = await fetch(`/data/${filename}`);
  const text = await response.text();
  return text;
}

function setupFileSystem(Module) {
  return Promise.all([
    fetchDataFile('pieces').then(data => {
      Module.FS.writeFile('pieces', data);
    }),
    fetchDataFile('centers').then(data => {
      Module.FS.writeFile('centers', data);
    }),
    fetchDataFile('kicks').then(data => {
      Module.FS.writeFile('kicks', data);
    }),
    fetch('/data/book_compressed.txt')
      .then(response => {
        if (!response.ok) { throw new Error('Compressed book not available'); }
        return response.text();
      })
      .then(data => {
        Module.FS.writeFile('book_compressed.txt', data);
      })
      .catch(() => {
        return fetchDataFile('book_100k.txt').then(data => {
          Module.FS.writeFile('book_100k.txt', data);
        });
      })
  ]);
}
