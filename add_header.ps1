$header = @"
/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

"@

$files = Get-ChildItem -Path "G:\codelabs_fork\codelabs_SchoolTimeTable\UI\SchoolTimeTable" -Include *.ets, *.ts -Recurse

foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw
    if ($null -eq $content) { $content = "" }
    
    if ($content.TrimStart().StartsWith("/*")) {
        if ($content -match "Licensed under the Apache License") {
            Write-Host "Skipping $($file.Name) - already has Apache license"
            continue
        }
        
        $endComment = $content.IndexOf("*/")
        if ($endComment -gt 0) {
            $content = $content.Substring($endComment + 2).TrimStart()
            Write-Host "Replacing header in $($file.Name)"
        }
    } else {
        Write-Host "Adding header to $($file.Name)"
    }
    
    $newContent = $header + "`n" + $content
    # Use UTF8NoBOM to match typical Linux/DevEco standards
    $utf8NoBom = New-Object System.Text.UTF8Encoding $false
    [System.IO.File]::WriteAllText($file.FullName, $newContent, $utf8NoBom)
}
