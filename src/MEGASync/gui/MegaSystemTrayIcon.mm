#include "MegaSystemTrayIcon.h"
#include <QtMacExtras>
#include <Cocoa/Cocoa.h>
#include <AppKit/AppKit.h>

@class MEGANSMenu;
@class MEGANSImageView;

@interface MEGANSStatusItem : NSObject
    {
@public
    MegaSystemTrayIcon *systray;
    NSStatusItem *item;
    QMenu *menu;
    bool menuVisible;
    NSImage *icon;
    NSImage *iconWhite;
    QPoint point;
    MEGANSImageView *imageCell;
}
-(id)initWithSysTray:(MegaSystemTrayIcon *)systray;
-(void)dealloc;
-(NSStatusItem*)item;
-(QRectF)geometry;
-(QPoint)point;
-(void)triggerSelector:(id)sender button:(Qt::MouseButton)mouseButton;
-(void)doubleClickSelector:(id)sender;
-(bool)isDarkModeEnabled;
@end

@interface MEGANSImageView : NSImageView {
    BOOL down;
    MEGANSStatusItem *parent;
}
-(id)initWithParent:(MEGANSStatusItem*)myParent;
-(void)menuTrackingDone:(NSNotification*)notification;
-(void)mousePressed:(NSEvent *)mouseEvent button:(Qt::MouseButton)mouseButton;
@end

@interface MEGANSMenu : NSMenu <NSMenuDelegate> {
    QPlatformMenu *qmenu;
}
-(QPlatformMenu*)menu;
-(id)initWithQMenu:(QPlatformMenu*)qmenu;
@end

class QSystemTrayIconSys
{
public:
    QSystemTrayIconSys(MegaSystemTrayIcon *sys) {
        item = [[MEGANSStatusItem alloc] initWithSysTray:sys];
    }
    ~QSystemTrayIconSys() {
        [[[item item] view] setHidden: YES];
        [item release];
    }

    void show() {
        [[[item item] view] setHidden: NO];
    }

    void hide() {
        [[[item item] view] setHidden: YES];
    }

    MEGANSStatusItem *item;
};

MegaSystemTrayIcon::MegaSystemTrayIcon()
{
    m_sys = new QSystemTrayIconSys(this);
}

MegaSystemTrayIcon::~MegaSystemTrayIcon()
{
    delete m_sys;
    m_sys = 0;
}

QRect MegaSystemTrayIcon::geometry() const
{
    if (!m_sys)
        return QRect();

    const QRectF geom = [m_sys->item geometry];
    if (!geom.isNull())
        return geom.toRect();
    else
        return QRect();
}

void MegaSystemTrayIcon::show()
{
    if(m_sys) m_sys->show();
}

void MegaSystemTrayIcon::hide()
{
    if(m_sys) m_sys->hide();
}

void MegaSystemTrayIcon::setIcon(const QIcon &icon, const QIcon &iconWhite)
{
    if (!m_sys)
        return;

    const bool menuVisible = m_sys->item->menu && m_sys->item->menuVisible;

    CGFloat hgt = [[[NSApplication sharedApplication] mainMenu] menuBarHeight];
    if(!hgt || hgt < 0 || hgt == 4)
    {
        hgt = 22;
    }
    const short scale = hgt - 4;

    const QIcon::Mode mode = menuVisible ? QIcon::Selected : QIcon::Normal;
    // request a pixmap with correct height and a large width, since the icon might be rectangular
    QPixmap pm = icon.pixmap(QSize(scale*10, scale), mode);
    QPixmap pmWhite = iconWhite.pixmap(QSize(scale*10, scale), mode);
    // the icon will be stretched over the full height of the menu bar
    // therefore we create a second pixmap which has the full height
    const qreal ratio = qApp->testAttribute(Qt::AA_UseHighDpiPixmaps) ? qApp->devicePixelRatio()
                                                                      : 1.0;
    QPixmap pixmap(pm.width(), hgt * ratio);
    QPixmap pixmapWhite(pmWhite.width(), hgt * ratio);
    if (!pm.isNull() && !pmWhite.isNull()) {
        pixmap.setDevicePixelRatio(pm.devicePixelRatio());
        pixmap.fill(Qt::transparent);
        QPainter p(&pixmap);
        p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                         QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
        QRect r = pm.rect();
        r.moveCenter(pixmap.rect().center());
        p.drawPixmap(r, pm);

        pixmapWhite.setDevicePixelRatio(pmWhite.devicePixelRatio());
        pixmapWhite.fill(Qt::transparent);
        QPainter pWhite(&pixmapWhite);
        p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing |
                         QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
        QRect rWhite = pmWhite.rect();
        rWhite.moveCenter(pixmapWhite.rect().center());
        pWhite.drawPixmap(rWhite, pmWhite);
    } else {
        pixmap = QPixmap(scale, scale);
        pixmap.fill(Qt::transparent);

        pixmapWhite = QPixmap(scale, scale);
        pixmapWhite.fill(Qt::transparent);
    }

#if QT_VERSION > 0x050000
    CGImageRef cgImage = QtMac::toCGImageRef(pixmap);
    CGImageRef cgImageWhite = QtMac::toCGImageRef(pixmapWhite);
#else
    CGImageRef cgImage = pixmap->toMacCGImageRef();
    CGImageRef cgImageWhite = pixmapWhite->toMacCGImageRef();
#endif

    if (cgImage != nil)
    {
        NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:cgImage];
        [m_sys->item->icon release];
        m_sys->item->icon = [[NSImage alloc] init];
        [m_sys->item->icon addRepresentation:bitmapRep];

        [bitmapRep release];
        CGImageRelease(cgImage);
    }

    if (cgImageWhite != nil)
    {
        NSBitmapImageRep *bitmapRepWhite = [[NSBitmapImageRep alloc] initWithCGImage:cgImageWhite];
        [m_sys->item->iconWhite release];
        m_sys->item->iconWhite =[[NSImage alloc] init];
        [m_sys->item->iconWhite addRepresentation:bitmapRepWhite];
        [bitmapRepWhite release];
        CGImageRelease(cgImageWhite);
    }

    [[[m_sys->item item] view] display];

}

void MegaSystemTrayIcon::setContextMenu(QMenu *menu)
{
    if (!m_sys)
        return;

    m_sys->item->menu = menu;
    if (menu && [m_sys->item->menu->toNSMenu() numberOfItems] > 0) {
        [[m_sys->item item] setHighlightMode:YES];
    } else {
        [[m_sys->item item] setHighlightMode:NO];
    }
}

void MegaSystemTrayIcon::setToolTip(const QString &toolTip)
{
    return;

    if (!m_sys)
        return;

    NSString *convertedString = [[NSString  alloc] initWithUTF8String:(char *)toolTip.toUtf8().constData()];
    [[[m_sys->item item] view] setToolTip:convertedString];
}

bool MegaSystemTrayIcon::isSystemTrayAvailable() const
{
    return true;
}

bool MegaSystemTrayIcon::supportsMessages() const
{
    return false;
}

QPoint MegaSystemTrayIcon::getPosition()
{
    return m_sys->item->point;
}

QMenu *MegaSystemTrayIcon::contextMenu()
{
    return NULL;
}

void MegaSystemTrayIcon::showMessage(const QString &title, const QString &message,
                                       const QIcon& icon, QSystemTrayIcon::MessageIcon, int)
{
    Q_UNUSED(title);
    Q_UNUSED(message);
    Q_UNUSED(icon);
    return;
}

//@implementation NSStatusItem (Qt)
//@end

@implementation MEGANSImageView
-(id)initWithParent:(MEGANSStatusItem*)myParent {
    self = [super init];
    parent = myParent;
    down = NO;
    return self;
}

-(void)menuTrackingDone:(NSNotification*)notification
{
    Q_UNUSED(notification);
    down = NO;
    parent->menuVisible = false;
    [self setNeedsDisplay:YES];
}

-(void)mousePressed:(NSEvent *)mouseEvent button:(Qt::MouseButton)mouseButton
{
    down = YES;
    int clickCount = [mouseEvent clickCount];
    [self setNeedsDisplay:YES];
    if (clickCount == 2) {
        [self menuTrackingDone:nil];
        [parent doubleClickSelector:self];
    } else {
        [parent triggerSelector:self button:mouseButton];
    }
}

-(void)mouseDown:(NSEvent *)mouseEvent
{
    [self mousePressed:mouseEvent button:Qt::LeftButton];
}

-(void)mouseUp:(NSEvent *)mouseEvent
{
    Q_UNUSED(mouseEvent);
    [self menuTrackingDone:nil];
}

- (void)rightMouseDown:(NSEvent *)mouseEvent
{
    [self mousePressed:mouseEvent button:Qt::RightButton];
}

-(void)rightMouseUp:(NSEvent *)mouseEvent
{
    Q_UNUSED(mouseEvent);
    [self menuTrackingDone:nil];
}

- (void)otherMouseDown:(NSEvent *)mouseEvent
{
    int num = [mouseEvent buttonNumber];
    Qt::MouseButton mouseButton = Qt::NoButton;
    switch(num)
    {
        case 0:
            mouseButton = Qt::LeftButton;
            break;
        case 1:
            mouseButton = Qt::RightButton;
            break;
        case 2:
            mouseButton = Qt::MiddleButton;
            break;
        default:
            if (num >= 3 && num <= 31)
                mouseButton = Qt::MouseButton(uint(Qt::MiddleButton) << (num - 3));
    }

    [self mousePressed:mouseEvent button:mouseButton];
}

-(void)otherMouseUp:(NSEvent *)mouseEvent
{
    Q_UNUSED(mouseEvent);
    [self menuTrackingDone:nil];
}

-(void)drawRect:(NSRect)rect {

    if([parent isDarkModeEnabled])
        [[[parent item] view] setImage: parent->iconWhite];
    else
        [[[parent item] view] setImage: parent->icon];

    [[parent item] drawStatusBarBackgroundInRect:rect withHighlight:down];
    [super drawRect:rect];
}
@end

@implementation MEGANSStatusItem

-(id)initWithSysTray:(MegaSystemTrayIcon *)sys
{
    self = [super init];
    if (self) {
        item = [[[NSStatusBar systemStatusBar] statusItemWithLength:NSSquareStatusItemLength] retain];
        menu = 0;
        icon = [[NSImage alloc] init];
        iconWhite = [[NSImage alloc] init];
        menuVisible = false;
        systray = sys;
        imageCell = [[MEGANSImageView alloc] initWithParent:self];
        [item setView: imageCell];
        [imageCell retain];
    }
    return self;
}

-(void)dealloc {
    [item setView: 0];
    [[NSStatusBar systemStatusBar] removeStatusItem:item];
    [imageCell release];
    [item release];
    [icon release];
    [iconWhite release];
    [super dealloc];
}

-(NSStatusItem*)item {
    return item;
}
-(QRectF)geometry {
    if (NSWindow *window = [[item view] window]) {
        NSRect screenRect = [[window screen] frame];
        NSRect windowRect = [window frame];
        return QRectF(windowRect.origin.x, screenRect.size.height-windowRect.origin.y-windowRect.size.height, windowRect.size.width, windowRect.size.height);
    }
    return QRectF();
}

- (void)triggerSelector:(id)sender button:(Qt::MouseButton)mouseButton {
    Q_UNUSED(sender);
    if (!systray)
        return;

    NSWindow *window = [[[NSApplication sharedApplication] currentEvent] window];
    NSRect rect = [window frame];
    point.setX(rect.origin.x);
    point.setY(rect.origin.y);

    if (mouseButton == Qt::MidButton)
        emit systray->activated(QSystemTrayIcon::MiddleClick);
    else
        emit systray->activated(QSystemTrayIcon::Trigger);

    if (menu) {
        NSMenu *m = menu->toNSMenu();
        [[NSNotificationCenter defaultCenter] addObserver:imageCell
         selector:@selector(menuTrackingDone:)
             name:NSMenuDidEndTrackingNotification
                 object:m];
        menuVisible = true;
        [item popUpStatusItemMenu: m];
    }
}

- (void)doubleClickSelector:(id)sender {
    Q_UNUSED(sender);
    if (!systray)
        return;
    emit systray->activated(QSystemTrayIcon::DoubleClick);
}

-(bool)isDarkModeEnabled
{
    bool returnValue = false;
    CFStringRef interfaceStyleKey = CFSTR("AppleInterfaceStyle");
    CFStringRef interfaceStyle = NULL;
    CFStringRef darkInterfaceStyle = CFSTR("Dark");
    interfaceStyle = (CFStringRef)CFPreferencesCopyAppValue(interfaceStyleKey,
                                                            kCFPreferencesCurrentApplication);
    if (interfaceStyle != NULL) {
        returnValue = (kCFCompareEqualTo == CFStringCompare(interfaceStyle, darkInterfaceStyle, 0));
        CFRelease(interfaceStyle);
    }
    return returnValue;
}

@end
