// 文件名: tree.ts
// 运行方式: 在终端输入 `node tree.ts`

const fs = require('fs');
const path = require('path');

const rootPath = __dirname;
const outputFile = path.join(rootPath, 'Project_Summary.txt');

// 配置忽略列表
const ignoreList = [
  'node_modules', 'build', '.ohpm', '.hvigor', '.cxx', '.idea', '.git',
  'oh_modules', '.preview', 'ohosTest', 'test', 'mock',
  'entrybackupability', 'rawfile', 'base', 'dark', 'element', 'media',
  'AppScope', 'resources', 'hvigorfile.ts', 'obfuscation-rules.txt',
  'code-linter.json5', 'local.properties', 'oh-package-lock.json5',
  'package-lock.json', '.gitignore', 'backup_config.json',
  'Project_Summary.txt', 'generate_context.ts', 'generate_context.js',
  'tree.ts' // 忽略自己
];

// 配置关键配置文件 (会读取内容)
const criticalConfigs = [
  'module.json5', 'app.json5', 'oh-package.json5',
  'build-profile.json5', 'main_pages.json'
];

// 需要统计行数的文件后缀
const codeExtensions = ['.ts', '.ets', '.js', '.json', '.json5', '.md', '.css'];

let totalLines = 0;      // 全局总行数计数器
let totalFiles = 0;      // 全局总文件数计数器

// 辅助函数：计算文件行数
function countFileLines(filePath) {
  try {
    const content = fs.readFileSync(filePath, 'utf-8');
    // 按换行符分割，兼容 Windows/Linux，过滤空行可选，这里统计所有行
    return content.split(/\r\n|\r|\n/).length;
  } catch (e) {
    return 0;
  }
}

/**
 * 核心递归函数
 * @returns {Object} { output: string, lines: number } 返回这一级及其子级的树状图字符串和总行数
 */
function generateTree(dir, prefix = '', isLast = true) {
  let name = path.basename(dir);
  let stats;

  // 递归结果对象
  let result = {
    output: '',
    lines: 0
  };

  try {
    stats = fs.statSync(dir);
  } catch (e) {
    return result; // 忽略无法读取的文件
  }

  const marker = isLast ? '└── ' : '├── ';

  // === 情况 1: 是文件 ===
  if (stats.isFile()) {
    totalFiles++;
    const size = stats.size;
    const ext = path.extname(name).toLowerCase();
    let lineInfo = '';
    let currentFileLines = 0;

    if (size === 0) {
      lineInfo = ' [EMPTY]';
    } else if (codeExtensions.includes(ext)) {
      currentFileLines = countFileLines(dir);
      totalLines += currentFileLines; // 加到全局统计
      result.lines = currentFileLines; // 记录当前文件行数
      lineInfo = ` [${currentFileLines} lines]`;
    }

    // 组装文件的行字符串
    result.output = `${prefix}${marker}${name}${lineInfo}\n`;
    return result;
  }

  // === 情况 2: 是文件夹 ===
  if (stats.isDirectory()) {
    let children = [];
    try {
      children = fs.readdirSync(dir).filter(child => !ignoreList.includes(child));
    } catch (e) {}

    // 排序：文件夹在前，文件在后
    children.sort((a, b) => {
      const aPath = path.join(dir, a);
      const bPath = path.join(dir, b);
      let aIsDir = false;
      let bIsDir = false;
      try { aIsDir = fs.statSync(aPath).isDirectory(); } catch(e){}
      try { bIsDir = fs.statSync(bPath).isDirectory(); } catch(e){}

      if (aIsDir && !bIsDir) return -1;
      if (!aIsDir && bIsDir) return 1;
      return a.localeCompare(b);
    });

    // 临时存储子节点的输出，因为我们需要先算完子节点的行数，才能在父节点行显示总数
    let childrenString = '';
    let dirTotalLines = 0;

    children.forEach((child, index) => {
      const isLastChild = index === children.length - 1;
      const childPrefix = isLast ? `${prefix}    ` : `${prefix}│   `;

      // 递归调用
      const childResult = generateTree(path.join(dir, child), childPrefix, isLastChild);

      dirTotalLines += childResult.lines;
      childrenString += childResult.output;
    });

    // 只有当文件夹内有代码行数时，才显示统计信息，避免视觉杂乱
    let dirInfo = dirTotalLines > 0 ? ` [DIR: ${dirTotalLines} lines]` : '';

    // 组装文件夹自己的行字符串
    result.output = `${prefix}${marker}${name}${dirInfo}\n` + childrenString;
    result.lines = dirTotalLines;

    return result;
  }

  return result;
}

// 1. 生成树状图 (接收返回的对象)
console.log('正在生成代码树并统计行数...');
const treeResult = generateTree(rootPath);
const treeOutput = treeResult.output;

// 2. 组装最终输出内容
let finalContent = '';

// --- 头部统计面板 ---
finalContent += '========================================\n';
finalContent += '       PROJECT CODE STATISTICS          \n';
finalContent += '========================================\n';
finalContent += `Total Files : ${totalFiles}\n`;
finalContent += `Total Lines : ${totalLines}\n`;
finalContent += '========================================\n\n';

// --- 树状图 ---
finalContent += '=== VISUAL TREE & LINE COUNTS ===\n';
finalContent += treeOutput;

// 3. 生成 JSON 文件列表 (保持原逻辑)
const fileList = [];
function scanFiles(dir) {
  try {
    const items = fs.readdirSync(dir);
    items.forEach(item => {
      if (ignoreList.includes(item)) return;
      const fullPath = path.join(dir, item);
      if (fs.statSync(fullPath).isDirectory()) {
        scanFiles(fullPath);
      } else {
        fileList.push(fullPath.replace(rootPath, '').replace(/\\/g, '/').substring(1));
      }
    });
  } catch (e) {}
}
scanFiles(rootPath);
finalContent += '\n=== JSON FILE LIST ===\n';
finalContent += JSON.stringify(fileList, null, 2);

// 4. 读取关键配置内容 (保持原逻辑)
finalContent += '\n\n=== CONFIG CONTENT ===\n';

function findAndReadConfigs(dir) {
  try {
    const items = fs.readdirSync(dir);
    items.forEach(item => {
      if (ignoreList.includes(item)) return;
      const fullPath = path.join(dir, item);
      if (fs.statSync(fullPath).isDirectory()) {
        findAndReadConfigs(fullPath);
      } else {
        if (criticalConfigs.includes(item)) {
          const relPath = fullPath.replace(rootPath, '').substring(1);
          const content = fs.readFileSync(fullPath, 'utf-8');
          finalContent += `\n--- FILE: ${relPath} ---\n${content}\n`;
        }
      }
    });
  } catch (e) {}
}
findAndReadConfigs(rootPath);

// 写入文件
fs.writeFileSync(outputFile, finalContent, 'utf-8');

console.log('--------------------------------------------------');
console.log(`✅ Project Summary Generated: ${outputFile}`);
console.log(`📊 Total Code Lines: ${totalLines}`);
console.log('--------------------------------------------------');