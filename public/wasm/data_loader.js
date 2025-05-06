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
    fetchDataFile('book_100k.txt').then(data => {
      Module.FS.writeFile('book_100k.txt', data);
    })
  ]);
}
