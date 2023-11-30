# CompileShaders.ps1

# Compile shaders using glslangValidator
Write-Host "Starting compilation..."

# Store the directory where the compiled .spv files will be placed
$compiledDir = Join-Path (Get-Location) "compiled"

# Create the directory if it doesn't exist
if (-not (Test-Path -Path $compiledDir -PathType Container)) {
    New-Item -Path $compiledDir -ItemType Directory
}

# Function to check if a file has been modified since the last compilation
function FileHasBeenModified($sourceFile, $compiledFile) {
    $sourceTime = (Get-Item $sourceFile).LastWriteTime
    $compiledTime = (Get-Item $compiledFile).LastWriteTime
    return $sourceTime -gt $compiledTime
}

# Get shader files
$shaderFiles = Get-ChildItem -Path . -File | Where-Object { $_.Extension -in @('.frag', '.vert', '.comp') }

# Iterate through shader files
foreach ($shaderFile in $shaderFiles) {
    $sourceFile = $shaderFile.FullName
    $compiledFile = Join-Path $compiledDir "$($shaderFile.BaseName)$($shaderFile.Extension).spv"

    # Check if the file has already been compiled
    if (-not (Test-Path -Path $compiledFile)) {
        # File has not been compiled
        Write-Host "$($shaderFile.Name) has not been compiled"
        # Compile the file
        & glslangValidator -V $sourceFile -o $compiledFile
    } else {
        # File has already been compiled
        Write-Host "$($shaderFile.Name) has already been compiled"

        # Check if the file has been modified since the last compilation
        if (FileHasBeenModified $sourceFile $compiledFile) {
            # File has been modified since last compilation
            Write-Host "$($shaderFile.Name) has been modified since last compilation"

            # Compile the file
            & glslangValidator -V $sourceFile -o $compiledFile
        }
    }
}

Write-Host "Compilation completed successfully."
