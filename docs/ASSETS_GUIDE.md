# 📸 Assets Management Guide

## 🎯 Overview

This guide explains how to manage images, diagrams, and other visual assets in the VibraGuard AI project documentation.

## 📁 Directory Structure

```
docs/assets/
├── architecture/          # System architecture diagrams
├── diagrams/              # ERD, sequence, flow diagrams
├── hardware/              # Hardware wiring and connection diagrams
├── screenshots/           # Application screenshots and photos
└── icons/                 # Icons, logos, and favicons
```

## 🖼️ Image Guidelines

### 📐 Diagrams & Architecture

- **Format:** PNG (preferred) or SVG
- **Resolution:** Minimum 1920x1080 for clarity
- **Size:** < 2MB per file (GitHub limit)
- **Naming:** `snake_case.png` (e.g., `system_architecture.png`)

### 📸 Screenshots

- **Format:** PNG or JPG
- **Resolution:** 1920x1080 or higher
- **Size:** < 1MB per file
- **Naming:** Descriptive names (e.g., `web_dashboard_home.png`)

### 🔧 Hardware Photos

- **Format:** JPG (smaller size)
- **Resolution:** 1280x720 minimum
- **Lighting:** Good lighting, no shadows
- **Naming:** `hardware_component_description.jpg`

## 📝 Adding New Assets

### Step 1: Choose Correct Directory

| Asset Type | Directory | Example |
|------------|-----------|---------|
| System architecture | `architecture/` | `system_overview.png` |
| Database diagrams | `diagrams/` | `erd_database.png` |
| Flow charts | `diagrams/` | `data_flow.png` |
| Sequence diagrams | `diagrams/` | `attack_detection_sequence.png` |
| Hardware wiring | `hardware/` | `esp32_mpu6050_wiring.png` |
| App screenshots | `screenshots/` | `mobile_app_dashboard.png` |
| Logos/Icons | `icons/` | `logo.png` |

### Step 2: Upload Images

#### Option A: Via GitHub Web Interface
1. Go to the target directory in GitHub
2. Click "Add file" → "Upload files"
3. Drag and drop your images
4. Commit with descriptive message

#### Option B: Via Terminal
```bash
# Copy images to correct directory
cp my_diagram.png docs/assets/diagrams/

# Add and commit
git add docs/assets/diagrams/my_diagram.png
git commit -m "docs(assets): add system architecture diagram"
```

### Step 3: Reference in Documentation

#### In README.md
```markdown
## 📸 Screenshots & Diagrams

### System Architecture
![System Architecture](docs/assets/architecture/system_architecture.png)
*Description of the diagram*
```

#### In Other Markdown Files
```markdown
<!-- Relative path from docs/ -->
![Database Schema](assets/diagrams/erd_database.png)

<!-- Or absolute path -->
![Database Schema](../docs/assets/diagrams/erd_database.png)
```

## 🛠️ Tools for Creating Diagrams

### Free Tools
- **Draw.io** (diagrams.net) - Flowcharts, ERD, architecture
- **Mermaid Live Editor** - Sequence diagrams, flowcharts
- **Excalidraw** - Hand-drawn style diagrams

### Professional Tools
- **Lucidchart** - Professional diagrams
- **Figma** - UI/UX and diagrams
- **Adobe Illustrator** - Vector graphics

## 📊 Best Practices

### ✅ Do's
- Use descriptive alt text for accessibility
- Optimize images for web (tinypng.com)
- Use consistent naming conventions
- Add captions explaining diagrams
- Keep file sizes under 2MB

### ❌ Don'ts
- Don't commit large PSD/AI source files
- Don't use generic names like `IMG_1234.jpg`
- Don't embed images in markdown without alt text
- Don't exceed GitHub's 2MB file limit

## 🔍 Image Optimization

### Online Tools
- **TinyPNG** - Lossless compression
- **ImageOptim** - Mac optimization
- **Caesium** - Cross-platform compression

### Command Line
```bash
# Install ImageMagick
# Ubuntu/Debian
sudo apt install imagemagick

# macOS
brew install imagemagick

# Optimize PNG
convert input.png -strip -quality 85 output.png

# Resize image
convert input.png -resize 1920x1080 output.png
```

## 📋 Checklist for New Assets

- [ ] Correct directory chosen
- [ ] Descriptive filename (snake_case)
- [ ] Optimized file size (< 2MB)
- [ ] Referenced in appropriate documentation
- [ ] Alt text provided
- [ ] Caption/description added
- [ ] Committed with clear message

## 🎨 Color Scheme & Branding

### Primary Colors
- **Blue:** #2563eb (ESP32, IoT)
- **Green:** #16a34a (Success, AI)
- **Red:** #dc2626 (Alerts, warnings)
- **Gray:** #6b7280 (Text, borders)

### Logo Usage
- Use `docs/assets/icons/logo.png` for branding
- Maintain aspect ratio
- Use appropriate sizes for different contexts

## 📞 Support

For questions about asset management:
- Check existing assets for examples
- Review this guide for best practices
- Create an issue for new asset requirements

---

**Last Updated:** November 2025